local extensions = {
    ["1"] = "ROFF",
    ["2"] = "ROFF",
    ["3"] = "ROFF",
    ["4"] = "ROFF",
    ["5"] = "ROFF",
    ["6"] = "ROFF",
    ["7"] = "ROFF",
    ["8"] = "ROFF",
    ["9"] = "ROFF",
    ada = "ADA",
    adb = "ADA",
    ads = "ADA",
    asd = "COMMONLISP",
    asm = "ASSEMBLY",
    auk = "AWK",
    automount = "INI",
    awk = "AWK",
    bash = "SHELL",
    bat = "BATCHFILE",
    bbl = "TEX",
    bib = "BIBTEX",
    btm = "BATCHFILE",
    c = "C",
    cc = "CPLUSPLUS",
    ["c++"] = "CPLUSPLUS",
    C = "CPLUSPLUS",
    cl = "COMMONLISP",
    clj = "CLOJURE",
    cls = "TEX",
    cmake = "CMAKE",
    cmd = "BATCHFILE",
    coffee = "COFFEESCRIPT",
    cpp = "CPLUSPLUS",
    cr = "RUBY",
    cs = "CSHARP",
    cson = "COFFEESCRIPT",
    css = "CSS",
    csv = "CSV",
    cxx = "CPLUSPLUS",
    dart = "DART",
    d = "D",
    desktop = "INI",
    di = "D",
    diff = "DIFF",
    doap = "XML",
    docbook = "XML",
    docker = "DOCKER",
    dot = "DOT",
    doxy = "DOXYGEN",
    dterc = "DTERC",
    dtx = "TEX",
    ebuild = "SHELL",
    el = "EMACSLISP",
    emacs = "EMACSLISP",
    eml = "EMAIL",
    eps = "POSTSCRIPT",
    flatpakref = "INI",
    flatpakrepo = "INI",
    frag = "GLSL",
    gawk = "AWK",
    gemspec = "RUBY",
    geojson = "JSON",
    glslf = "GLSL",
    glsl = "GLSL",
    glslv = "GLSL",
    gnuplot = "GNUPLOT",
    go = "GO",
    gperf = "GPERF",
    gp = "GNUPLOT",
    gpi = "GNUPLOT",
    groovy = "GROOVY",
    gv = "DOT",
    h = "C",
    H = "CPLUSPLUS",
    hh = "CPLUSPLUS",
    hpp = "CPLUSPLUS",
    hs = "HASKELL",
    htm = "HTML",
    html = "HTML",
    hxx = "CPLUSPLUS",
    ini = "INI",
    ins = "TEX",
    java = "JAVA",
    js = "JAVASCRIPT",
    json = "JSON",
    ksh = "SHELL",
    l = "LEX",
    lsp = "COMMONLISP",
    ltx = "TEX",
    lua = "LUA",
    m4 = "M4",
    make = "MAKE",
    mak = "MAKE",
    markdown = "MARKDOWN",
    mawk = "AWK",
    md = "MARKDOWN",
    mkd = "MARKDOWN",
    mkdn = "MARKDOWN",
    mk = "MAKE",
    m = "OBJECTIVEC",
    moon = "MOONSCRIPT",
    mount = "INI",
    nawk = "AWK",
    nginxconf = "NGINX",
    nginx = "NGINX",
    nim = "NIM",
    ninja = "NINJA",
    nix = "NIX",
    page = "MALLARD",
    patch = "DIFF",
    path = "INI",
    pc = "PKGCONFIG",
    perl = "PERL",
    php = "PHP",
    pl = "PERL",
    pls = "INI",
    plt = "GNUPLOT",
    pm = "PERL",
    po = "GETTEXT",
    postscript = "POSTSCRIPT",
    pot = "GETTEXT",
    proto = "PROTOBUF",
    ps = "POSTSCRIPT",
    py3 = "PYTHON",
    py = "PYTHON",
    rake = "RUBY",
    rb = "RUBY",
    rdf = "XML",
    rktd = "RACKET",
    rktl = "RACKET",
    rkt = "RACKET",
    rockspec = "LUA",
    rs = "RUST",
    rst = "RESTRUCTUREDTEXT",
    s = "ASSEMBLY",
    S = "ASSEMBLY",
    scala = "SCALA",
    scm = "SCHEME",
    scss = "SCSS",
    sed = "SED",
    service = "INI",
    sh = "SHELL",
    sld = "SCHEME",
    slice = "INI",
    sls = "SCHEME",
    socket = "INI",
    sql = "SQL",
    ss = "SCHEME",
    sty = "TEX",
    svg = "XML",
    target = "INI",
    tcl = "TCL",
    texinfo = "TEXINFO",
    texi = "TEXINFO",
    tex = "TEX",
    timer = "INI",
    toml = "TOML",
    topojson = "JSON",
    ts = "TYPESCRIPT",
    tsx = "TYPESCRIPT",
    ui = "XML",
    vala = "VALA",
    vapi = "VALA",
    vcard = "VCARD",
    vcf = "VCARD",
    vert = "GLSL",
    ver = "VERILOG",
    vhdl = "VHDL",
    vhd = "VHDL",
    vh = "VHDL",
    vim = "VIML",
    v = "VERILOG",
    wsgi = "PYTHON",
    xhtml = "HTML",
    xml = "XML",
    xsd = "XML",
    xslt = "XML",
    xsl = "XML",
    yaml = "YAML",
    yml = "YAML",
    y = "YACC",
    zsh = "SHELL",
}

return {
    keys = extensions,
    function_name = "filetype_from_extension",
    return_type = "FileTypeEnum",
    default_return = 0,
}
