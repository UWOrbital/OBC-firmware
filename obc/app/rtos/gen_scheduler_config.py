from jinja2 import Environment, FileSystemLoader
from toml import load


class Task:
    def __init__(self, stem: str, stack_size: str, priority: str):
        self.stem = stem
        self.stack_size = stack_size
        self.priority = priority
        self.camel_case = self.stem_to_camel(self.stem)

        self.name = "\"" + stem + "\""
        if self.stem == "digital_watchdog_mgr":
            self.init = "NULL"
        else:
            self.init = "obcTaskInit" + self.camel_case
        self.function = "obcTaskFunction" + self.camel_case
        self.stack = "obcTaskStack" + self.camel_case
        self.buffer = "obcTaskBuffer" + self.camel_case
        self.config_id = "OBC_SCHEDULER_CONFIG_ID_" + self.stem.upper()

    def stem_to_camel(self, stem: str) -> str:
        lst = stem.split("_")
        return "".join(word.capitalize() for word in lst)


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
    with open(config_filename, "r") as file:
        toml_data = load(file)

    for t in toml_data["tasks"]:
        task_list.append(Task(t["stem"], t["stack_size"], t["priority"])) 
    
    with open(output_filename, mode="w") as output:
        output.write(template.render(context))
        print("Generated", output_filename)


if __name__ == "__main__":
    main()
