import re

translations = {
    '\\': "_backslash",
    '_': "__",
    "<": "_lt",
    ">": "_gt",
    '"': "_quote",
    "'": "_squote",
    "(": "_po",
    ")": "_pc",
    "-": "_minus",
    "=": "_equal",
    "|": "_pipe",
    "/": "_slash",
    ".": "_dot",
    "\0": "_null",
    ":": "_colon",
    ";": "_semicolon",
    "*": "_star",
    "[": "_sbo",
    "]": "sbc",
    ",": "_comma",
    "?": "_question",
    "\n": "_newline"
}

keys = translations.keys()
keys = map(lambda x: re.escape(x), keys)

pattern = re.compile('|'.join(keys))

translationsInv = {v: k for k, v in translations.items()}

keys = translationsInv.keys()
keys = map(lambda x: re.escape(x), keys)

patternInv = re.compile('|'.join(keys))


def escapePath(path):
    return pattern.sub(lambda x: translations[x.group()], path)


def unescapePath(path):
    return patternInv.sub(lambda x: translationsInv[x.group()], path)
