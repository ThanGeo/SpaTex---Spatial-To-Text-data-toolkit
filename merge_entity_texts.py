import sys
from collections import defaultdict

def merge_lines_by_common_prefix(input_file, output_file):
    with open(input_file, 'r') as file:
        lines = file.readlines()
    
    # Dictionary to hold prefix-to-content mappings
    prefix_map = defaultdict(list)

    for line in lines:
        # Find the first colon (:) to separate prefix from content
        if ":" in line:
            prefix, content = line.split(":", 1)
            prefix_map[prefix.strip()].append(content.strip())

    # Merge lines with the same prefix
    merged_lines = [
        f"{prefix}: {' '.join(contents)}"
        for prefix, contents in prefix_map.items()
    ]

    # Write the result to the output file
    with open(output_file, 'w') as file:
        file.write("\n".join(merged_lines) + '\n')

if __name__ == "__main__":
    # Check for correct number of arguments
    if len(sys.argv) != 3:
        print("Usage: python script.py <input_file> <output_file>")
        sys.exit(1)

    # Read input and output filenames from command-line arguments
    input_file = sys.argv[1]
    output_file = sys.argv[2]

    # Call the function
    merge_lines_by_common_prefix(input_file, output_file)
