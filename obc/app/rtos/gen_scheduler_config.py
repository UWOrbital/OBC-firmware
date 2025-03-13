from jinja2 import Environment, FileSystemLoader
from toml import load


class Task:
    def __init__(self, task_name: str, stack_size: str, priority: str,
                 function_stem: str, config_id_stem: str) -> None:
        self.task_name = task_name
        self.stack_size = stack_size
        self.priority = priority
        self.function_stem = function_stem
        self.config_id_stem = config_id_stem


task_list = []

template_filename = "config_skeleton.c.jinja"
config_filename = "scheduler_config.toml"
output_filename = "obc_scheduler_config.c"

env = Environment(loader=FileSystemLoader(""), trim_blocks=True, lstrip_blocks=True)
template = env.get_template(template_filename)
context = {
    "tasks": task_list,
}


def main() -> None:
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
