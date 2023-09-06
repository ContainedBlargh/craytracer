import random
import json
import sys

# Function to generate a random float between min_val and max_val
def random_float(min_val, max_val):
    return min_val + (max_val - min_val) * random.random()

# Function to generate a random RGB color
def random_color():
    return {
        "r": random_float(0.0, 1.0),
        "g": random_float(0.0, 1.0),
        "b": random_float(0.0, 1.0)
    }

# Function to generate a random position within specified bounds
def random_position(min_x, max_x, min_y, max_y, min_z, max_z):
    return {
        "x": random_float(min_x, max_x),
        "y": random_float(min_y, max_y),
        "z": random_float(min_z, max_z)
    }

# Check if the number of balls argument is provided
if len(sys.argv) != 2:
    print("Usage: python generate_scene.py <num_balls>")
    sys.exit(1)

try:
    num_balls = int(sys.argv[1])
except ValueError:
    print("Error: Please provide a valid integer for the number of balls.")
    sys.exit(1)

# Initialize the scene dictionary
scene = {
    "camera": {
        "position": {
            "x": 0.0,
            "y": 0.0,
            "z": 0.0
        },
        "direction": {
            "x": 0.0,
            "y": 0.0,
            "z": 1.0
        }
    },
    "lights": [
        {
            "position": {
                "x": 0.0,
                "y": 0.0,
                "z": 0.0
            },
            "color": {
                "r": 1.0,
                "g": 1.0,
                "b": 1.0
            }
        }
    ],
    "shapes": []
}

# Generate random spheres and add them to the scene
for _ in range(num_balls):
    ball = {
        "type": "sphere",
        "color": random_color(),
        "radius": 0.05,
        "center": random_position(-1.5, 1.5, -1.0, 0.0, 0.2, 100.0)
    }
    scene["shapes"].append(ball)


# Save the scene to a JSON file
output_filename = f"scene_{num_balls}_balls.json"
with open(output_filename, "w") as json_file:
    json.dump(scene, json_file, indent=4)

print(f"Scene JSON file generated: {output_filename}")
