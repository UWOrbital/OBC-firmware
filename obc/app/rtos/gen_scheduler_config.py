from dataclasses import dataclass
from jinja2 import Environment, FileSystemLoader
from toml import load


@dataclass
class Task:
    """
    @brief Class for storing task data strings

    @attribute task_name (str) - name of task; snake_case
    @attribute stack_size (str) - stack size, stored as 32-bit unsigned int; xU 
    @attribute priority (str) - task priority as 32-bit unsigned int; xU
    @attribute function_stem (str) - base name for task functions; CamelCase
    @attribute config_id_stem (str) - config id; SNAKE_CASE
    """
    task_name: str
    stack_size: str
    priority: str
    function_stem: str
    config_id_stem: str


task_list: list[Task] = []

template_filename = "config_skeleton.c.jinja"
config_filename = "scheduler_config.toml"
output_filename = "obc_scheduler_config.c"

env = Environment(loader=FileSystemLoader(""), trim_blocks=True, lstrip_blocks=True)
template = env.get_template(template_filename)
context = {
    "tasks": task_list,
}


def main() -> None:
    """Entry point to script"""
    with open(config_filename, "r") as config:
        toml_data = load(config)

    for t in toml_data["tasks"]:
        task_list.append(Task(t["task_name"], t["stack_size"], t["priority"],
                              t["function_stem"], t["config_id_stem"])) 
    
    with open(output_filename, "w") as output:
        output.write(template.render(context))
        print(f"Generated new scheduler config at ./{output_filename}")


if __name__ == "__main__":
    main()
