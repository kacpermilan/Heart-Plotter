import os

def generate_cpp_stringlist():
    # Get the current directory
    current_directory = os.getcwd()

    # Get the list of .dat files in the current directory
    files = [f for f in os.listdir(current_directory) if f.endswith('.dat') and os.path.isfile(os.path.join(current_directory, f))]

    # Generate the C++ QStringList initialization code
    cpp_code = 'QStringList files = QStringList()'
    for file in files:
        file_without_extension = file[:-4]  # Remove the '.dat' extension
        cpp_code += ' << "{}"'.format(file_without_extension.replace('\\', '\\\\'))  # Escape backslashes for C++ string
    cpp_code += ';'

    return cpp_code

# Generate and print the C++ code
cpp_code = generate_cpp_stringlist()
print(cpp_code)
