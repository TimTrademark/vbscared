from typing import List
from pathlib import Path


class ExfiltrateItem:
    def __init__(self, type: str, name: str, content):
        self.type = type
        self.name = name
        self.content = content

def parse_exfiltrate_input(input: str):
    items = []
    splitted = input.split("EXF~t~")[1:]
    for item in splitted:
        type = item[0]
        sp = item.split("~")
        name = sp[1]
        content = "".join(sp[2:])
        items.append(ExfiltrateItem(type, name, content))
    return items

def recreate_filesystem(root_folder: str, items: List[ExfiltrateItem]):
    Path(root_folder).mkdir(parents=True, exist_ok=True)
    for i in items:
        if i.type == 'd':
            Path(root_folder + "/" + i.name.replace(":","")).mkdir(parents=True, exist_ok=True)
        else:
            with open(root_folder + "/" + i.name.replace(":",""), 'w') as f:
                f.write(i.content)