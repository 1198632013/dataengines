isEmpty(PRO_LIBRARY_BASENAME) {
    PRO_LIBRARY_BASENAME = lib
}

# Project Source Path
PRO_SOURCE_TREE = $$PWD
# Project 3rdparty Path
PRO_THREEPARTY_PATH = $$PRO_SOURCE_TREE/3rdparty
# Project Shard File Path
PRO_SOURCE_SHARD = $$PRO_SOURCE_TREE/shard
# Project Scripts File Path
PRO_SOURCE_SCRIPTS = $$PRO_SOURCE_TREE/scripts

# Project Build Path
isEmpty(PRO_BUILD_TREE) {
    sub_dir = $$_PRO_FILE_PWD_
    sub_dir ~= s,^$$re_escape($$PWD),,
    PRO_BUILD_TREE = $$clean_path($$OUT_PWD)
    PRO_BUILD_TREE ~= s,$$re_escape($$sub_dir)$,,
}
# Project Bin Path
PRO_BIN_PATH = $$PRO_BUILD_TREE/bin
