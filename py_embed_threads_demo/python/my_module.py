import threading


class Worker:
    def __init__(self, name: str):
        self._name = name

    def greet(self, who: str) -> str:
        tid = threading.get_ident()
        return f"Hello {who} from {self._name} (PyTID={tid})"

    def compute(self, a: int, b: int) -> int:
        return int(a) + int(b)

    def might_fail(self, x: int) -> int:
        x = int(x)
        if x % 2 == 1:
            raise ValueError(f"Odd input not allowed: {x}")
        return x * 2