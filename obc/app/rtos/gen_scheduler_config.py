from re import match

from jinja2 import Environment, FileSystemLoader
from pydantic import BaseModel, NonNegativeInt, PositiveInt, ValidationError, field_validator
from toml import load
from typing import Optional


class Task(BaseModel):
    """
    @brief Class for storing task data

    @attribute task_name (str) - name of task; snake_case
    @attribute stack_size (PositiveInt) - stack size as 32-bit unsigned int
    @attribute priority (NonNegativeInt) - task priority as 32-bit unsigned int
    @attribute function_stem (str) - base name for task functions; CamelCase
    @attribute config_id_stem (str) - config id; SNAKE_CASE
    @attribute conditional_enable (Optional[str]) - conditional macro; SNAKE_CASE
    @attribute task_init (bool) - whether task has init function
    """
    task_name: str
    stack_size: str
    priority: str
    function_stem: str
    config_id_stem: str
    conditional_enable: Optional[str] = None
    task_init: bool = True

    @field_validator('task_name', mode='after')
    @classmethod
    def is_snake_case(cls, name: str) -> str:
        if match(r"^[a-z]+(_[a-z]+)*$", name):
            return name
        raise ValueError(f"{name} must be lower snake_case")

    @field_validator('function_stem', mode='after')
    @classmethod
    def is_camel_case(cls, name: str) -> str:
        if match(r"^([A-Z][a-z]*)+$", name):
            return name
        raise ValueError(f"{name} must be upper CamelCase")

    @field_validator('config_id_stem', 'conditional_enable', mode='after')
    @classmethod
    def is_upper_snake_case(cls, name: str) -> str:
        if match(r"[A-Z]+(_[A-Z]+)*$", name):
            return name
        raise ValueError(f"{name} must be upper SNAKE_CASE")


TASK_LIST: list[Task] = []

C_TEMPLATE_FILENAME = "config_skeleton.c.jinja"
H_TEMPLATE_FILENAME = "header_skeleton.h.jinja"
CONFIG_FILENAME = "scheduler_config.toml"
C_FILENAME = "obc_scheduler_config.c"
H_FILENAME = "obc_scheduler_config.h"


def main() -> None:
    """Entry point to script"""
    env = Environment(loader=FileSystemLoader(""), trim_blocks=True,
                      lstrip_blocks=True)

    c_template = env.get_template(C_TEMPLATE_FILENAME)
    h_template = env.get_template(H_TEMPLATE_FILENAME)

    context = {
        "tasks": TASK_LIST,
    }

    with open(CONFIG_FILENAME, "r") as config:
        toml_data = load(config)

    for t in toml_data["tasks"]:
        try:
            TASK_LIST.append(Task(**t))
        except ValidationError as e:
            print(e)
    
    with open(C_FILENAME, "w") as c_output:
        c_output.write(c_template.render(context))
        print(f"Generated new scheduler config at ./{C_FILENAME}")
    
    with open(H_FILENAME, "w") as h_output:
        h_output.write(h_template.render(context))
        print(f"Generated new header file at ./{H_FILENAME}")


if __name__ == "__main__":
    main()
