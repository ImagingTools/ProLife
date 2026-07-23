import re, html, json

MSG_RE = re.compile(r'(<message[^>]*>)(.*?)(</message>)', re.S)
SRC_RE = re.compile(r'<source>(.*?)</source>', re.S)
TR_RE  = re.compile(r'<translation([^>]*)>(.*?)</translation>', re.S)

def xml_escape(s):
    # Escape only &, <, > for element content. Keep quotes/apostrophes literal.
    return s.replace('&','&amp;').replace('<','&lt;').replace('>','&gt;')

def raw_to_key(raw_source):
    # Unescape XML entities to get plain-text key
    return html.unescape(raw_source)

def iter_messages(content):
    for m in MSG_RE.finditer(content):
        yield m

def get_source(mbody):
    s = SRC_RE.search(mbody)
    return s.group(1) if s else None

def get_translation(mbody):
    t = TR_RE.search(mbody)
    if not t: return None, None
    return t.group(1), t.group(2)
