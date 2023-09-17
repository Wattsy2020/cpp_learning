import re
from argparse import ArgumentParser
from pathlib import Path


def strip_parens(word: str) -> str:
    return word[1:-1] if word[0] == '(' and word[-1] == ')' else word


def reformat_to_add_assert_equal(code: str) -> str:
    assert_pattern = re.compile(r"\s*assert\((.*) == (.*)\)")
    formatted_lines: list[str] = []
    for line in code.split("\n"):
        if match := re.match(assert_pattern, line):
            left, right = map(strip_parens, match.groups())
            formatted_lines.append(f"ctest::assert_equal({left}, {right});")
        else:
            formatted_lines.append(line)
    return "\n".join(formatted_lines)


def add_ctest_assert_equal() -> None:
    parser = ArgumentParser()
    parser.add_argument('filename', type=str, help='name of file to format')
    args = parser.parse_args()
    filepath = Path(args.filename).absolute()

    code_text = filepath.read_text()
    reformatted_text = reformat_to_add_assert_equal(code_text)
    filepath.write_text(reformatted_text)


if __name__ == "__main__":
    add_ctest_assert_equal()
