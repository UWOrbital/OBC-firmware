import os
import re


class MissingReturnTypeError(Exception):
    """Exception raised for missing return type cases."""

    pass


def parse_functions_from_header(header_file: str) -> list[tuple[str, str, list[str]]]:
    """Parses function declarations from a header file."""
    functions = []
    try:
        with open(header_file, encoding="utf-8", errors="ignore") as file:
            content = file.read()
            # Regex to find function declarations, improved to account for spaces
            matches = re.findall(r"(\w[\w\s\*]+)\s+(\w+)\s*\((.*?)\)\s*;", content)
            for ret_type, func_name, params in matches:
                param_list = params.split(",") if params else []
                # Strip spaces from parameter types and names
                param_list = [p.strip() for p in param_list]
                functions.append((ret_type.strip(), func_name.strip(), param_list))

    except Exception as e:
        print(f"Error reading {header_file}: {e}")  # Error handling
    return functions


def get_return_value(ret_type: str) -> str:
    """Determine the return value based on the return type."""
    if "obc_error_code_t" in ret_type:
        return "    return OBC_ERR_CODE_SUCCESS;"
    elif "void" in ret_type:
        return ""  # No return statement for void
    elif "pointer" in ret_type or "*" in ret_type:  # Check for pointer types
        return "    return NULL;"
    elif "bool" in ret_type:
        return "    return true;"
    elif any(x in ret_type for x in ["int", "uint", "uint8_t", "uint16_t", "uint32_t", "DSTATUS", "DRESULT"]):
        return "    return 0;"
    else:
        raise MissingReturnTypeError(f"Missing return type case for: {ret_type}")


def clean_param(param: str) -> str:
    """Removes pointer indicators (*) and array indicators ([]) from a parameter."""
    # Extract the actual variable name, ignoring * and []
    return param.replace("*", "").replace("[]", "").split()[-1]


def write_no_op_functions(folder: str, functions: list[tuple[str, str, list[str]]], header_files: list[str]) -> None:
    """Writes no-op functions to a file for each driver, including UNUSED for each parameter."""
    folder_name = os.path.basename(folder).upper()
    output_file = os.path.join(folder, f"{folder_name.lower()}_stubs.c")

    with open(output_file, "w") as f:
        f.write(f"#ifndef CONFIG_{folder_name}\n")
        f.write('#include "obc_errors.h"\n')
        f.write('#include "obc_general_util.h"\n')

        # Include all header files in the folder
        for header_file in header_files:
            f.write(f'#include "{header_file}"\n')
        f.write("\n")  # New line after includes

        for ret_type, func_name, params in functions:
            param_list = ", ".join(params)
            """ Generate unused variable statements with just the variable names
                (ignore "void", "*" and array brackets "[]") """
            unused_params = "\n".join([f"    UNUSED({clean_param(param)});" for param in params if param != "void"])
            try:
                return_value = get_return_value(ret_type)  # Determine the return value
                f.write(f"{ret_type} {func_name}({param_list}) {{\n")
                f.write(unused_params + "\n" if unused_params else "")
                f.write(return_value + "\n")
                f.write("}\n\n")
            except MissingReturnTypeError as e:
                print(e)  # Log the exception for missing return type cases

        f.write(f"#endif // CONFIG_{folder_name}\n")

    print(f"Wrote no-op functions to {output_file}")  # Debugging statement


def process_driver_folders(drivers_dir: str) -> None:
    """Processes driver folders and generates no-op stubs for each folder."""
    for folder_name in os.listdir(drivers_dir):
        if folder_name == "rm46":  # Ignore the rm46 folder
            continue
        folder_path = os.path.join(drivers_dir, folder_name)
        if os.path.isdir(folder_path):
            functions = []
            header_files = []
            for file in os.listdir(folder_path):
                if file.endswith(".h"):
                    header_file = os.path.join(folder_path, file)
                    header_files.append(file)  # Collect the header file name
                    functions.extend(parse_functions_from_header(header_file))
            if functions:
                write_no_op_functions(folder_path, functions, header_files)
            else:
                print(f"No functions found in folder: {folder_path}")  # Debugging statement


drivers_dir = "./"  # Change to the path of your drivers directory
process_driver_folders(drivers_dir)
