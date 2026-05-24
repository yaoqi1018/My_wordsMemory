"""
convert_doc.py  ——  将 .doc 词汇表转换为 UTF-8 编码的 .txt 文件。
不依赖 Microsoft Word，使用 olefile 库直接读取 OLE 复合文档。

用法：python convert_doc.py
依赖：pip install olefile
"""
import os
import olefile


DATA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")

LEVELS = [
    ("primary", "小学英语词汇表.doc", "words_primary.txt"),
    ("junior",  "初中英语词汇表.doc", "words_junior.txt"),
    ("senior",  "高中英语词汇表.doc", "words_senior.txt"),
]


# ---------- 从 .doc 提取文本 ----------

def extract_text(doc_path):
    ole = olefile.OleFileIO(doc_path)
    data = ole.openstream("WordDocument").read()
    ole.close()
    return data.decode("utf-16-le", errors="ignore")


# ---------- 词汇解析 ----------

def has_phonetic(line):
    return "/" in line or ("[" in line and "]" in line)


def parse_slash_line(line):
    """例: pen /pen/ 钢笔"""
    last = line.rfind("/")
    if last == -1:
        return None
    first = line.rfind("/", 0, last)
    if first == -1:
        return None
    eng = line[:first].strip()
    pho = "/" + line[first + 1:last].strip() + "/"
    chn = line[last + 1:].strip()
    return (eng, pho, chn) if eng and chn else None


def parse_bracket_line(line):
    """例: 1 what [hwɔt] pron 什么"""
    left  = line.find("[")
    right = line.find("]")
    if left == -1 or right == -1 or left >= right:
        return None

    before = line[:left].strip()
    space = before.find(" ")
    if space != -1 and before[:space].isdigit():
        before = before[space + 1:].strip()

    eng  = before
    pho  = "[" + line[left + 1:right].strip() + "]"

    after = line[right + 1:].strip()
    dot = after.find(".")
    sp  = after.find(" ")
    if dot != -1 and dot < 7:
        after = after[dot + 1:].strip()
    if sp != -1 and sp < 7:
        w = after[:sp]
        if w.isalpha() and len(w) <= 6:
            after = after[sp + 1:].strip()

    chn = after
    return (eng, pho, chn) if eng and chn else None


def parse_line(line):
    if not has_phonetic(line):
        return None
    if "/" in line:
        return parse_slash_line(line)
    return parse_bracket_line(line)


# ---------- 主流程 ----------

def convert_one(level_name, doc_file, txt_file):
    doc_path = os.path.join(DATA_DIR, doc_file)
    txt_path = os.path.join(DATA_DIR, txt_file)

    if os.path.exists(txt_path):
        print(f"[跳过] {level_name}: txt 已存在")
        return

    if not os.path.exists(doc_path):
        print(f"[跳过] {level_name}: doc 不存在")
        return

    print(f"[转换] {level_name}: {doc_file} ...")
    text = extract_text(doc_path)

    words = []
    for line in text.replace("\n", "\r").split("\r"):
        line = line.strip()
        if not line:
            continue
        r = parse_line(line)
        if r:
            words.append(r)

    if not words:
        print(f"[警告] {level_name}: 未解析到任何单词！")
        return

    with open(txt_path, "w", encoding="utf-8") as f:
        for eng, pho, chn in words:
            f.write(f"{eng}\t{pho}\t{chn}\n")

    print(f"[完成] {level_name}: {len(words)} 个单词 -> {txt_file}")


def main():
    print("=" * 50)
    print("词汇表 .doc -> .txt 转换工具")
    print("=" * 50)
    for lv, doc, txt in LEVELS:
        convert_one(lv, doc, txt)
    print("全部完成。")


if __name__ == "__main__":
    main()
