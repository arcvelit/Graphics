import sys
import os

def process_obj_file(file_path):
    output_lines = []
    with open(file_path, 'r') as f:
        for line in f:
            if line.startswith('f '):
                # Split face line into individual vertex indices
                vertices = line.strip().split()[1:]
                # Process each vertex index
                new_vertices = []
                for vertex in vertices:
                    # Split vertex into parts
                    parts = vertex.split('/')
                    # Keep only the vertex and normal indices
                    new_vertex = '/'.join([parts[0], parts[2]])
                    new_vertices.append(new_vertex)
                # Concatenate vertices and form new face line
                new_line = 'f ' + ' '.join(new_vertices) + '\n'
                output_lines.append(new_line)
            else:
                output_lines.append(line)
    
    # Write modified OBJ file
    output_file_path = os.path.splitext(file_path)[0] + '.obj'
    with open(output_file_path, 'w') as f:
        f.writelines(output_lines)

def main():
    # Check if a file path is provided as an argument
    if len(sys.argv) != 2:
        print("Usage: simplify.py <obj_file_path>")
        return
    
    file_path = sys.argv[1]
    
    # Check if the file exists
    if os.path.exists(file_path):
        process_obj_file(file_path)
        print("Modified OBJ file created successfully.")
    else:
        print("File not found.")

if __name__ == "__main__":
    main()
