# Sometimes it just makes sense to bundle up all your C program code into a single file before compiling.
# The reason we do this is to avoid multiple definitions when using macros to define generics.

from io import FileIO
import sys
import os
from argparse import ArgumentParser, ArgumentTypeError
import re
from re import Match
from typing import List, Tuple
import networkx as nx
from networkx import DiGraph
from glob import glob
import matplotlib.pyplot as plt
from networkx.drawing.nx_agraph import graphviz_layout

class CObject:
    def __init__(self, path: str, local: bool):
        self.path = path
        self.is_local = local
        self.basename = os.path.basename(path)
        self.is_header = self.basename.endswith(".h")
        self.W = 0
        if self.is_header and local:
            source_name = path.removesuffix(".h") + ".c"
            source_paths = list(set(list(glob(source_name)) + list(glob(f"**/{source_name}"))))
            self.has_source = len(source_paths) > 0
            self.source_path = None if not self.has_source else source_paths[0]
        else:
            self.has_source = False
            self.source_path = path
        pass

    def __eq__(self, __value: object) -> bool:
        try:
            return (
                self.basename == __value.basename
                and self.is_local == __value.is_local
                and self.is_header == __value.is_header
            )
        except:
            return False

    def __hash__(self) -> int:
        return hash((self.basename, self.is_local, self.is_header))

    def __str__(self) -> str:
        return f'"{self.basename}"' if self.is_local else f"<{self.basename}>"
    
    def __repr__(self) -> str:
        return str(self)

    def region(self) -> Tuple[str]:
        name = re.sub('[\\_\/\.]', '_', self.path).upper()
        start = f"#pragma region {name}"
        end = f"#pragma endregion {name}"
        return start, end


global_include_re = re.compile(r"^\s*#include\s+<(.+)>")
local_include_re = re.compile(r"^\s*#include\s+\"(.+)\"")

def direct_dependencies(fp: FileIO) -> List[Tuple[bool, CObject]]:
    deps = []
    for line in fp:
        global_m: Match[str] = global_include_re.match(line)
        if global_m is not None:
            path = global_m.group(1)
            deps.append((False, path))

        local_m: Match[str] = local_include_re.match(line)
        if local_m is not None:
            path = local_m.group(1)
            deps.append((True, path))
    return deps

def construct_network(
    G: DiGraph, current: CObject
) -> DiGraph:
    """
    Construct an unweighted graph to figure out where all the dependencies are.
    """
    if not current.is_local or (G.has_node(current) and G.out_degree(current) > 0):
        return G
    
    deps = []
    with open(current.path) as fp:
        deps += direct_dependencies(fp)
    if current.has_source:
        with open(current.source_path) as fp:
            deps += direct_dependencies(fp)
    deps = [dep for dep in set(deps) if dep != current.path and dep != current.basename]
    children = [CObject(dep, local) for local, dep in deps]
    for child in children:
        G.add_edge(current, child)
    for child in children:
        G = construct_network(G, child)
    return G

def bundle_source(G: DiGraph, fp: FileIO):
    # Step 1: Compute the number of descendants for each node
    descendant_count = {node: len(nx.descendants(G, node)) for node in G.nodes()}

    # Step 2: Group nodes into layers based on descendant count
    layered_nodes = {}
    for node, count in descendant_count.items():
        if count not in layered_nodes:
            layered_nodes[count] = [node]
        else:
            layered_nodes[count].append(node)

    view_layered_nodes = [', '.join(map(str, layered_nodes[layer])) for layer in layered_nodes]
    
    gbl_includes = set()

    def filter_line_by_line(in_fp: FileIO, out_fp: FileIO):
        for line in in_fp:
            gbl_include = global_include_re.match(line)
            if gbl_include is not None and gbl_include.group(1) not in gbl_includes:
                gbl_includes.add(gbl_include.group(1))
                out_fp.write(line)
            elif local_include_re.match(line) is None:
                out_fp.write(line)

    # Step 3: Loop over each layer and write the header and source
    for layer in sorted(layered_nodes.keys()):
        for node in layered_nodes[layer]:
            c_obj: CObject = node
            if not c_obj.is_local:
                continue
            r_start, r_end = c_obj.region()
            fp.write(f'\n{r_start}\n')
            with open(c_obj.path) as hfp:
                filter_line_by_line(hfp, fp)
            fp.write("\n")
            if c_obj.has_source:
                with open(c_obj.source_path) as cfp:
                    filter_line_by_line(cfp, fp)
            fp.write(f'\n{r_end}\n')
            

def input_file(path):
    if not os.path.isfile(path):
        raise ArgumentTypeError(f"{path} is not a valid input file")
    return path


def main():
    ap = ArgumentParser(
        sys.argv[0],
        description="Bundles C programs into a single file before compiling.\nThis will all make sense.",
    )
    ap.add_argument(
        "main_path",
        type=input_file,
        help="The path to the file that contains your C `main` function.",
    )
    ap.add_argument("output_path", help="Path to the output bundled C file.")
    args = ap.parse_args()
    abs_output_path = os.path.abspath(args.output_path)
    abs_main_path = os.path.abspath(args.main_path)

    old_dir = os.getcwd()
    new_dir = os.path.dirname(args.main_path)

    os.chdir(new_dir)
    g = construct_network(DiGraph(), CObject(abs_main_path, True))
    with open(abs_output_path, "w") as fp:
        bundle_source(g, fp)
    os.chdir(old_dir)
    

if __name__ == "__main__":
    main()
