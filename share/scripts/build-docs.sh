#!/bin/sh

CWD=`pwd`
PANDOC=${PANDOC:-pandoc}
PANDOC_OPT=${PANDOC_OPT:-"-f markdown -t html4 -o"}
PANDOC_CMD=${PANDOC_CMD:-"${PANDOC} ${PANDOC_OPT}"}
CYAN_VERSION=`cat ${CWD}/CMakeLists.txt | sed '/Cyan VERSION/!d;s/)//' | awk '{print $3}'`
HTML_PATH="${CWD}/docs/html"
MD_PATH="${CWD}/docs"
DOCS="ChangeLog Thanks Support"
HTML_HEADER="
<html><head><style>
body { font-family: Arial, Verdana, Helvetica; font-size: 10pt; }
h1, h2 { font-weight: normal; }
ul li a { text-decoration: none; }
</style></head><body>"
HTML_FOOTER="</body></html>"

#HTML_MENU="<h2>Cyan ${CYAN_VERSION}</h2>"
HTML_MENU="<hr>"
#HTML_MENU="${HTML_MENU}&nbsp;&nbsp;[<a href='index.html'>Index</a>]"
for menu in ${DOCS}; do
    HTML_MENU="${HTML_MENU}&nbsp;&nbsp;[<a href='${menu}.html' style='text-decoration:none;font-weight:bold;'>${menu}</a>]"
done
HTML_MENU="${HTML_MENU}<hr>"
HTML_HEADER="${HTML_HEADER}${HTML_MENU}"

HTML_INDEX="${HTML_PATH}/index.html"
HTML_INDEX_SRC="
${HTML_HEADER}
<p>Welcome to the documentation for Cyan ${CYAN_VERSION}.</p>
${HTML_FOOTER}"

if [ ! -d "${HTML_PATH}" ]; then
    mkdir -p "${HTML_PATH}" || exit 1
fi

#echo ${HTML_INDEX_SRC} > "${HTML_INDEX}" || exit 1
for doc in $DOCS; do
    if [ -f "${MD_PATH}/${doc}.md" ]; then
        echo $HTML_HEADER > "${HTML_PATH}/${doc}.html" || exit 1
        CONVERT="${PANDOC_CMD} ${HTML_PATH}/${doc}-TEMP.html ${MD_PATH}/${doc}.md"
        ${CONVERT} || exit 1
        cat "${HTML_PATH}/${doc}-TEMP.html" >> "${HTML_PATH}/${doc}.html" || exit 1
        rm -f "${HTML_PATH}/${doc}-TEMP.html" || exit 1
        echo $HTML_FOOTER >> "${HTML_PATH}/${doc}.html" || exit 1
        #echo "<li><a href='${doc}.html'>${doc}</a></li>" >> "${HTML_INDEX}" || exit 1
    fi
done

echo "Done!"
