from typing import Any


class Singleton(type):
    """a singleton metaclass"""

    _instances: dict[Any, Any] = {}

    def __call__(cls: Any, *args: Any, **kwargs: Any) -> Any:  # noqa: ANN401
        """return same instance every instantiation"""
        if cls not in cls._instances:
            cls._instances[cls] = super().__call__(*args, **kwargs)
        return cls._instances[cls]
