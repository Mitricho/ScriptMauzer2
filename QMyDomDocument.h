#include <QtXml/QDomDocument>
#include <QMap>

class QMyDomDocument : public QDomDocument {
public:
    QMyDomDocument() : QDomDocument() {}
    QMyDomDocument(const QString& name) : QDomDocument(name) {}
    QMyDomDocument(const QDomDocumentType& doctype) : QDomDocument(doctype) {}

    QDomElement elementById(const QString& id)
    {
        if (map.contains(id)) {
            QDomElement e = map[id];
            if (e.parentNode().nodeType() != QDomNode::BaseNode) {
                return e;
            }

            map.remove(id);
        }

        bool res = this->find(this->documentElement(), id);
        if (res) {
            return map[id];
        }

        return QDomElement();
    }

private:
    QMap<QString, QDomElement> map;

    bool find(QDomElement node, const QString& id)
    {
        if (node.hasAttribute("id")) {
            QString value = node.attribute("id");
            this->map[value] = node;
            if (value == id) {
                return true;
            }
        }

        for (int i=0; i<node.childNodes().length(); ++i) {
            QDomNode n = node.childNodes().at(i);
            if (n.isElement()) {
                bool res = this->find(n.toElement(), id);
                if (res) {
                    return true;
                }
            }
        }

        return false;
    }
};
