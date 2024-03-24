/*---------------------------------------------------------------------------------------*/
#include <QFile>
#include <QColor>
#include <QXmlQuery>
#include <QRegExp>
#include <QRgb>
#include <QtCore>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QTextCodec>
#include <QDebug>
#include <QSize>
#include <QCryptographicHash>
#include <QStringList>
#include <QDomDocument>
#include "QMyDomDocument.h"
#include <QCoreApplication>
#include <QScriptEngine>
#include <QScriptValue>
#include <QSettings>
#include <QLibraryInfo>
#include <QScriptValueIterator>
#include <QStringBuilder>
#include <QImage>
#include <QProcess>
#include <QSize>
#include <QSvgRenderer>
#include <QPainter>
#include <QGraphicsSvgItem>

//#include "quazip/quazip/quazip.h"
//#include "quazip/quazip/quazipfile.h"

#include <errno.h>
#include <stdio.h>
#ifdef _WIN32
    #include <windows.h>
#endif

#define LODWORD(l)   ((DWORD)((DWORDLONG)(l)))

namespace anima
{
/*---------------------------------------------------------------------------------------*/
    QMyDomDocument doc;
/*---------------------------------------------------------------------------------------*/
    QString slh(QLatin1String("/"));
    QSettings settings("Atech","Fabrika");//,QSettings::IniFormat

    QDir::Filters AllDirsAndFiles = QDir::Dirs |
            QDir::Files |
            QDir::NoSymLinks |
            QDir::NoDotAndDotDot |
            QDir::NoDot |
            QDir::NoDotDot |
            QDir::Readable;

    QDir::Filters FilesOnly = QDir::Files |
            QDir::NoSymLinks |
            QDir::NoDotAndDotDot |
            QDir::NoDot |
            QDir::NoDotDot |
            QDir::Readable;

    QDir::Filters DirsOnly = QDir::Dirs |
            QDir::NoSymLinks |
            QDir::NoDotAndDotDot |
            QDir::NoDot |
            QDir::NoDotDot |
            QDir::Readable;
/*--------------------------------------------------------------------------------------------------------*/
QString fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm)
{
    QFile sourceFile(fileName);
    qint64 fileSize = sourceFile.size();
    const qint64 bufferSize = 10240;

    if (sourceFile.open(QIODevice::ReadOnly))
    {
        char buffer[bufferSize];
        int bytesRead;
        int readSize = qMin(fileSize, bufferSize);

        QCryptographicHash hash(hashAlgorithm);
        while (readSize > 0 && (bytesRead = sourceFile.read(buffer, readSize)) > 0)
        {
            fileSize -= bytesRead;
            hash.addData(buffer, bytesRead);
            readSize = qMin(fileSize, bufferSize);
        }

        sourceFile.close();
        return QString(hash.result().toHex());
    }
    return QString();
}
/*--------------------------------------------------------------------------------------------------------*/
/* ?????????? ???? ??? ????? ? ?????????? \a ? ????????? ? ?????? \b */
void recurseAddDir(QDir d, QStringList & list) {

    QStringList qsl = d.entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);

    foreach (QString file, qsl) {

        QFileInfo finfo(QString("%1/%2").arg(d.path()).arg(file));

        if (finfo.isSymLink())
            return;

        if (finfo.isDir()) {

            QString dirname = finfo.fileName();
            QDir sd(finfo.filePath());

            recurseAddDir(sd, list);

        } else
            list << QDir::toNativeSeparators(finfo.filePath());
    }
}
/*---------------------------------------------------------------------------------------*/
//https://stackoverflow.com/questions/2598117/zipping-a-folder-file-using-qt
/*bool unzip(const QString & filePath, const QString & extDir, const QString & singleFileName)
{
    QString extDirPath;
    if(extDir.isEmpty()){
        QFileInfo fi(filePath);
        QDir absDir = fi.absoluteDir();
        extDirPath = QDir::toNativeSeparators(QDir::cleanPath(absDir.absolutePath()) + "/");
        qDebug() << "out dir " << extDirPath;
    }else{
        extDirPath = extDir;
    }

    QuaZip zip(filePath);

    if (!zip.open(QuaZip::mdUnzip)) {
        qWarning("Unable to open archive: %d", zip.getZipError());
        return false;
    }

    zip.setFileNameCodec("IBM866");

    //qWarning("%d entries\n", zip.getEntriesCount());
    qWarning("Global comment: %s\n", zip.getComment().toLocal8Bit().constData());

    QuaZipFileInfo info;

    QuaZipFile file(&zip);

    QFile out;
    QString name;
    char c;
    for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile()) {

        if (!zip.getCurrentFileInfo(&info)) {
            qWarning("Archive error: %d\n", zip.getZipError());
            return false;
        }

        if (!singleFileName.isEmpty())
            if (!info.name.contains(singleFileName))
                continue;

        if (!file.open(QIODevice::ReadOnly)) {
            qWarning("Failed to open file: %d", file.getZipError());
            return false;
        }

        name = QString("%1/%2").arg(extDirPath).arg(file.getActualFileName());

        if (file.getZipError() != UNZ_OK) {
            qWarning("testRead(): file.getFileName(): %d", file.getZipError());
            return false;
        }

        //out.setFileName("out/" + name);
        out.setFileName(name);

        // this will fail if "name" contains subdirectories, but we don't mind that
        out.open(QIODevice::WriteOnly);
        // Slow like hell (on GNU/Linux at least), but it is not my fault.
        // Not ZIP/UNZIP package's fault either.
        // The slowest thing here is out.putChar(c).
        while (file.getChar(&c)) out.putChar(c);

        out.close();

        if (file.getZipError() != UNZ_OK) {
            qWarning("Cannot get file name: %d", file.getZipError());
            return false;
        }

        if (!file.atEnd()) {
            qWarning("File read error: read all but not EOF");
            return false;
        }

        file.close();

        if (file.getZipError() != UNZ_OK) {
            qWarning("Error reading archive: %d", file.getZipError());
            return false;
        }
    }

    zip.close();

    if (zip.getZipError() != UNZ_OK) {
        qWarning("testRead(): zip.close(): %d", zip.getZipError());
        return false;
    }

    return true;
}*/
/*---------------------------------------------------------------------------------------*/
/*bool zip(const QString & filePath, const QDir & dir, const QString & comment)
{
    QuaZip zip(filePath);
    zip.setFileNameCodec("IBM866");

    if (!zip.open(QuaZip::mdCreate)) {
        qWarning() << QString("Failed to open: %1").arg(zip.getZipError());
        return false;
    }

    if (!dir.exists()) {
        //qWarning(QString("%1").arg(dir.absolutePath()));
        return false;
    }

    QFile inFile;

    // ???????? ?????? ?????? ? ????? ??????????
    QStringList sl;
    recurseAddDir(dir, sl);

    // ??????? ?????? ????????? ?? QFileInfo ????????
    QFileInfoList files;
    foreach (QString fn, sl) files << QFileInfo(fn);

    QuaZipFile outFile(&zip);

    char c;
    foreach(QFileInfo fileInfo, files) {

        if (!fileInfo.isFile())
            continue;

        // ???? ???? ? ?????????????, ?? ????????? ??? ???? ????????????? ? ?????? ??????
        // ????????: fileInfo.filePath() = "D:\Work\Sources\SAGO\svn\sago\Release\tmp_DOCSWIN\Folder\123.opn"
        // ????? ????? ???????? ????? ?????? fileNameWithSubFolders ????? ????? "Folder\123.opn" ? ?.?.
        QString fileNameWithRelativePath = fileInfo.filePath().remove(0, dir.absolutePath().length() + 1);

        inFile.setFileName(fileInfo.filePath());

        if (!inFile.open(QIODevice::ReadOnly)) {
            qWarning() << QString("testCreate(): inFile.open(): %1").arg(inFile.errorString().toLocal8Bit().constData());
            return false;
        }

        if (!outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileNameWithRelativePath, fileInfo.filePath()))) {
            qWarning() << QString("Failed to open: %1").arg(outFile.getZipError());
            return false;
        }

        while (inFile.getChar(&c) && outFile.putChar(c));

        if (outFile.getZipError() != UNZ_OK) {
            qWarning() << QString("Archive error: %1").arg(outFile.getZipError());
            return false;
        }

        outFile.close();

        if (outFile.getZipError() != UNZ_OK) {
            qWarning() << QString("Failed to close: %1").arg(outFile.getZipError());
            return false;
        }

        inFile.close();
    }

    // + ???????????
    if (!comment.isEmpty())
        zip.setComment(comment);

    zip.close();

    if (zip.getZipError() != 0) {
        qWarning() << QString("Unable to close archive: %1").arg(zip.getZipError());
        return false;
    }

    return true;
}*/
/*---------------------------------------------------------------------------------------*/
QString getVersionString(QString fName)
{
#ifdef _WIN32
    //qDebug() << "getVersionString for " << fName;

    DWORD dwHandle;
    DWORD dwLen = GetFileVersionInfoSize(fName.toStdWString().c_str(), &dwHandle);

    // GetFileVersionInfo
    LPVOID lpData = new BYTE[dwLen];
    if(!GetFileVersionInfo(fName.toStdWString().c_str(), dwHandle, dwLen, lpData)){
        //qDebug() << "error in GetFileVersionInfo";
        delete[] lpData;
        return "";
    }

    VS_FIXEDFILEINFO *lpBuffer = NULL;
    UINT uLen;

    if(!VerQueryValue(lpData,QString("\\").toStdWString().c_str(),(LPVOID*)&lpBuffer,&uLen)){
        qDebug() << "error in VerQueryValue";
        delete[] lpData;
        return "";
    }

    /* language ID 040904E4: U.S. English, char set = Windows, Multilingual
    LPTSTR lptstr;
    UINT BufLen;
    if (!VerQueryValueW(lpData,
                        TEXT("\\StringFileInfo\\040904E4\\ProductVersion"),
                        (LPVOID*)&lptstr,
                        (PUINT)&BufLen))
    {
        qDebug() << "FileVersion: not found";
    }
    else{
        qDebug() << "FileVersion:" << QString::fromWCharArray(lptstr);
    }*/


    return QString::number( (lpBuffer->dwFileVersionMS >> 16 ) & 0xffff ) + "." +
    QString::number( ( lpBuffer->dwFileVersionMS) & 0xffff ) + "." +
    QString::number( ( lpBuffer->dwFileVersionLS >> 16 ) & 0xffff ) + "." +
    QString::number( ( lpBuffer->dwFileVersionLS) & 0xffff );
#endif
    Q_UNUSED(fName)
    return QString();
}
/*---------------------------------------------------------------------------------------*/
QScriptValue getFileVer(QScriptContext *context, QScriptEngine *engine){
    if(context->argumentCount() > 0)
    {
        QString path = context->argument(0).toString();
        return engine->toScriptValue(getVersionString(path));
    }
    else
    {
        return engine->toScriptValue(QString());
    }
}
/*---------------------------------------------------------------------------------------*/
    QScriptValue btoa(QScriptContext *context, QScriptEngine *engine)
    {
        if(context->argumentCount() > 0){
            if(context->argument(0).isString())
            {
                QString data = context->argument(0).toString();
                QFileInfo fi(data);
                if(fi.isFile() && fi.isReadable())
                {
                    if(fi.suffix() == QString("png") || fi.suffix() == QString("jpg") || fi.suffix() == QString("jpeg") || fi.suffix() == QString("webp"))
                    {
                        QImage pic(data);
                        if(context->argument(1).isNumber() && context->argument(2).isNumber()){
                            //qDebug() << "Writing scaled image as base64";
                            qsreal w = context->argument(1).toNumber();
                            qsreal h = context->argument(2).toNumber();
                            if(int(w) > 0 && int(h)>0){
                               pic.scaled(int(w),int(h),Qt::KeepAspectRatio).swap(pic);
                            }
                        }
                        QByteArray byteArray;
                        QBuffer buffer(&byteArray);
                        pic.save(&buffer,fi.suffix().toUpper().toLocal8Bit());
                        return engine->toScriptValue(QString::fromLatin1(byteArray.toBase64().data()));
                    }else{
                        qDebug() << "btoa: wrong file - " << data.left(100);
                        QByteArray b = data.toUtf8();
                        return engine->toScriptValue(QString(b.toBase64()));
                    }
                }
                else
                {
                    QByteArray b = data.toUtf8();
                    return engine->toScriptValue(QString(b.toBase64()));
                }
            }
            else{
                return engine->toScriptValue(QString());
            }
        }else{
            return engine->toScriptValue(QString());
        }
    }
/*---------------------------------------------------------------------------------------*/
    const char* stringToChar(const QString &str){
        QByteArray barr = str.toLocal8Bit();
        const char *swp = barr.data();
        return swp;
    }
/*---------------------------------------------------------------------------------------*/
    bool writeFile(const QString &path, QString textdata, QString codecName)
    {
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            return false;
        }
        QTextStream out(&file);
        out.setGenerateByteOrderMark(true);
        out.setCodec(stringToChar(codecName));

        QRegExp ex("\r");
        textdata.replace(ex,QString(""));

        out << textdata;
        out.flush();
        file.close();
        return true;
    }
/*--------------------------------------------------------------------------------------*/
    QString cleanStr(const QString &str,bool isFilename = false, bool isPath = false)
    {
        QString clKey = str.simplified().trimmed(); //Qt::CaseInsensitive
        QString r("");
        clKey.replace(QString("{"),r);
        clKey.replace(QString("}"),r);
        clKey.replace(QString("#"),r);
        clKey.replace(QString("!"),r);
        clKey.replace(QString("~"),r);
        clKey.replace(QString("("),r);
        clKey.replace(QString(")"),r);
        clKey.replace(QString("+"),r);
        clKey.replace(QString(":"),r);
        if(!isPath)clKey.replace(QString("/"),r);
        if(!isPath)clKey.replace(QString("\\"),r);
        clKey.replace(QString("%"),r);
        clKey.replace(QString("$"),r);
        clKey.replace(QString("@"),r);
        clKey.replace(QString("^"),r);
        clKey.replace(QString("&"),r);
        clKey.replace(QString("?"),r);
        clKey.replace(QString("*"),r);
        clKey.replace(QString("|"),r);
        clKey.replace(QString("["),r);
        clKey.replace(QString("]"),r);
        clKey.replace(QString(";"),r);
        clKey.replace(QString("<"),r);
        clKey.replace(QString(">"),r);
        clKey.replace(QString("`"),r);
        clKey.replace(QString("="),r);
        clKey.replace(QString("?"),r);
        clKey.replace(QString("'"),r);
        clKey.replace(QString("\""),r);
        clKey.replace(QString("?").toLatin1(),r);
        clKey.replace(QString(":"),r);
        clKey.replace(QString(","),r);
        if(isFilename){
            //clKey.replace(QString("-"),QString("-"));
            clKey.replace(QString(" "),QString("-"));
        }
        return clKey;
    }
/*---------------------------------------------------------------------------------------*/
    QString translit(QString ret)
    {
        ret.replace(QString("?"),QString("a"), Qt::CaseSensitive);
        ret.replace(QString("?"),QString("b"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("v"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("g"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("d"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("e"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("yo"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("zh"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("z"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("i"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("i"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("k"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("l"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("m"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("n"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("o"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("p"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("r"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("s"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("t"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("u"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("f"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("ch"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("z"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("ch"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("sh"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("ch"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString(""),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("y"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString(""),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("ye"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("yu"),Qt::CaseSensitive);
        ret.replace(QString("?"),QString("ya"),Qt::CaseSensitive);

        ret.replace("?","A",Qt::CaseSensitive);
        ret.replace("?","B",Qt::CaseSensitive);
        ret.replace("?","V",Qt::CaseSensitive);
        ret.replace("?","G",Qt::CaseSensitive);
        ret.replace("?","D",Qt::CaseSensitive);
        ret.replace("?","E",Qt::CaseSensitive);
        ret.replace("?","YE",Qt::CaseSensitive);
        ret.replace("?","ZH",Qt::CaseSensitive);
        ret.replace("?","Z",Qt::CaseSensitive);
        ret.replace("?","I",Qt::CaseSensitive);
        ret.replace("?","I",Qt::CaseSensitive);
        ret.replace("?","K",Qt::CaseSensitive);
        ret.replace("?","L",Qt::CaseSensitive);
        ret.replace("?","M",Qt::CaseSensitive);
        ret.replace("?","N",Qt::CaseSensitive);
        ret.replace("?","O",Qt::CaseSensitive);
        ret.replace("?","P",Qt::CaseSensitive);
        ret.replace("?","R",Qt::CaseSensitive);
        ret.replace("?","S",Qt::CaseSensitive);
        ret.replace("?","T",Qt::CaseSensitive);
        ret.replace("?","U",Qt::CaseSensitive);
        ret.replace("?","F",Qt::CaseSensitive);
        ret.replace("?","CH",Qt::CaseSensitive);
        ret.replace("?","Z",Qt::CaseSensitive);
        ret.replace("?","CH",Qt::CaseSensitive);
        ret.replace("?","SH",Qt::CaseSensitive);
        ret.replace("?","CH",Qt::CaseSensitive);
        ret.replace("?","",Qt::CaseSensitive);
        ret.replace("?","Y",Qt::CaseSensitive);
        ret.replace("?","",Qt::CaseSensitive);
        ret.replace("?","YE",Qt::CaseSensitive);
        ret.replace("?","YU",Qt::CaseSensitive);
        ret.replace("?","YA",Qt::CaseSensitive);

        //qDebug() << QString("translit string: %1").arg(ret);

        return ret;
    }
/*---------------------------------------------------------------------------------------*/
    QString validatePath(const QString &loadFile, QScriptEngine *engine)
    {
        //qDebug() << QString("Validate path: %1").arg(loadFile);
        QString currentFileName = engine->globalObject().property("qs").property("script").property("absoluteFilePath").toString();
        QUrl url(currentFileName);
        QUrl path = url.resolved(loadFile);
        return path.toString();
    }
/*---------------------------------------------------------------------------------------*/
    QString newFileExt(const QString &path, const QString &newExt){
        if(!path.isEmpty() && !newExt.isEmpty()){
            QFileInfo fi(path);
            QDir folder = fi.absoluteDir();
            QString parentDir = folder.absolutePath() % slh;
            if(!QFileInfo(parentDir).exists())folder.mkpath(parentDir);
            return parentDir % fi.baseName() % "." % newExt;
        }else{
            qDebug() << "Error: newFileExt: missing arguments";
            return QString();
        }
    }
/*---------------------------------------------------------------------------------------*/
    QScriptValue imageColor(QScriptContext *context, QScriptEngine *engine)
    {
        /*  ????????????? ?? JavaScript:
            imageColor(path, xPos="left",yPos="top");
            ????:
            imageColor(path, xPos=20,yPos=38);*/

        QString def("#000000");
        if(context->argumentCount() > 0){
            if(context->argument(0).isString())
            {
                QString path = context->argument(0).toString();
                QString loadFile = validatePath(path, engine);
                if(QFileInfo(loadFile).exists())
                {
                    QImage pic;
                    if(pic.load(loadFile))
                    {
                       if(context->argumentCount() > 2)
                       {
                           int ix = 0;
                           int yg = 0;
                           if(context->argument(1).isString() &&
                              context->argument(2).isString())
                           {
                               QString xPos = context->argument(1).toString();
                               QString yPos = context->argument(2).toString();

                               if(xPos.toLower() == "left"){
                                   ix = 0;
                               }else if(xPos.toLower() == "center"){
                                   ix = qRound(qreal(pic.width())/qreal(2));
                               }else if(xPos.toLower() == "right"){
                                   ix = pic.width() - 1;
                               }

                               if(yPos.toLower() == "top"){
                                   yg = 0;
                               }else if(yPos.toLower() == "middle"){
                                   yg = qRound(qreal(pic.height())/qreal(2));
                               }else if(yPos.toLower() == "bottom"){
                                   yg = pic.height() - 1;
                               }
                           }else if(context->argument(1).isNumber() &&
                                    context->argument(2).isNumber())
                           {
                               ix = int(context->argument(1).toNumber());
                               yg = int(context->argument(2).toNumber());
                           }

                           if(ix >= 0 && ix < pic.width() && yg < pic.height() && yg >= 0){
                              QPoint p(ix,yg);
                              QRgb color = pic.pixel(p);
                              QColor c(color);
                              return engine->toScriptValue(c.name());
                           }else{
                               qDebug() << QString("imageColor: Out of range: x:%1, y:%2").arg(ix).arg(yg);
                           }
                       }
                    }else{
                        qDebug() << QString("imageColor: Failed to load %1").arg(loadFile);
                        return engine->toScriptValue(def);
                    }
                }else{
                qDebug() << QString("imageColor: File not found %1").arg(loadFile);
                return engine->toScriptValue(def);
            }
            }
        }
        qDebug() << "imageColor: Bad arguments. Returning default color";
        return engine->toScriptValue(def);
    }
/*---------------------------------------------------------------------------------------*/
    QScriptValue getSumForFile(QScriptContext *context, QScriptEngine *engine){
        if(context->argumentCount() > 0)
        {
            QString path = context->argument(0).toString();
            QString loadFile = validatePath(path, engine);
            QFile theFile(loadFile);
            QByteArray thisFile;
            if (theFile.open(QIODevice::ReadOnly))
            {
                thisFile = theFile.readAll();
            }
            else
            {
                qDebug() << "Failed to open " << loadFile;
            }
            QByteArray ba = QCryptographicHash::hash((thisFile), QCryptographicHash::Md5).toHex();
            char *c_str2 = ba.data();
            return engine->toScriptValue(QString(c_str2));
        }
        else
        {
            return engine->toScriptValue(QString());
        }
    }
/*---------------------------------------------------------------------------------------*/
    QScriptValue processImage(QScriptContext *context, QScriptEngine *engine)
    {
        //processImage(loadFile, width=-1,height=-1, crop=false, format='PNG', removeSourceFile, outFile);

        if(context->argumentCount() > 0)
        {
            QString path = context->argument(0).toString();
            QString loadFile = validatePath(path, engine);
            if(context->argumentCount() > 2)
            {
                int quality = -1;
                QString format = "PNG";
                QString outFile;
                QImage result;
                bool crop = false;
                qsreal width  = -1;
                qsreal height = -1;
                if(context->argument(1).isNumber() &&
                   context->argument(2).isNumber())
                {
                    width = context->argument(1).toNumber();
                    height = context->argument(2).toNumber();
                }
                if(context->argumentCount() > 3){
                    if(context->argument(3).isBool()){
                        crop = context->argument(3).toBool();
                    }
                }
                QImage pic(loadFile);
                if((pic.width() != width || pic.height() != height) &&
                    width != -1 && height != -1)
                {
                    if(crop && (pic.width() > width || pic.height() > height)){
                        width  = qMin(pic.width(),int(width));
                        height = qMin(pic.height(),int(height));
                        result = pic.copy(0,0,width,height);
                    }else{
                        width  = qMax(pic.width(),int(width));
                        height = qMax(pic.height(),int(height));
                        result = pic.scaled(QSize(width,height),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
                    }
                }else{
                    result = pic;
                }
                if(!result.isNull()){
                    if(context->argumentCount() > 4){
                        if(context->argument(4).isString()){
                            format = context->argument(4).toString();
                        }
                    }
                    const char* ext;
                    if(format.toUpper() == "PNG"){
                       ext = "png";
                    }else if(format.toUpper() == "JPG" || format.toUpper() == "JPEG"){
                       ext = "jpg";
                       quality = 80;
                    }else if(format.toUpper() == "BMP"){
                       ext = "bmp";
                    }else if(format.toUpper() == "TIFF"){
                       ext = "tiff";
                    }else{
                       ext = "png";
                       qDebug() << "Setting default ext 'PNG'";
                    }

                    if(context->argumentCount() > 5){
                        if(context->argument(5).isBool()){
                            bool removeSourceFile = context->argument(5).toBool();
                            if(removeSourceFile){
                                pic = QImage();
                                //qDebug() << "loadFile=" + loadFile;
                                QFileInfo fi(loadFile);
                                if(fi.exists()){
                                    QFile f(loadFile);
                                    bool deleted = f.remove();
                                    if(!deleted){
                                        qDebug() << QString("Failed to delete image '%1' as was requested").arg(loadFile);
                                    }
                                }else{
                                    qDebug() << QString("Image file '%1' not found").arg(loadFile);
                                }
                            }
                        }
                    }
                    if(context->argumentCount() > 6){
                        if(context->argument(6).isString()){
                            QString p = context->argument(6).toString();
                            outFile = validatePath(p, engine);
                        }
                    }else{
                        QString ex = QString(ext);
                        if(QFileInfo(loadFile).suffix() != ex){
                            outFile = newFileExt(loadFile,ex);
                        }else{
                            outFile = loadFile.append(".").append(ext);
                        }
                    }
                    //qDebug() << QString("Saving file as '%1'").arg(outFile);
                    result.save(outFile,ext,quality);
                }else{
                    qDebug() << QString("Failed to process image '%1'").arg(loadFile);
                }
            }
        }
        return engine->toScriptValue(false);
    }
/*---------------------------------------------------------------------------------------*/
    QString _readTextFile(QString path)
    {
        QString contents = "";
        QFile file(path);
        if (!file.open(QFile::ReadOnly)) {
            //qDebug() << QString("Failed to load file: %1").arg(path);
        }else{
            QTextStream stream(&file);
            QString line = stream.readAll();
            QByteArray ba = line.toUtf8();
            QTextCodec *codec = QTextCodec::codecForName("UTF-8");
            contents = codec->toUnicode(ba);
            file.close();
            stream.flush();
        }
        return contents;
    }
/*---------------------------------------------------------------------------------------*/
    QString _loadTextFile(QScriptContext *context, QScriptEngine *engine)
    {
        QString contents = "";
        if(context->argumentCount() > 0)
        {
            QString path = context->argument(0).toString();
            QString loadFile = validatePath(path, engine);
            contents = _readTextFile(loadFile);
        }else{
            qDebug() << QString("loadTextFile: Wrong number of arguments: %1").arg(context->argumentCount());
        }
        return contents;
    }
/*---------------------------------------------------------------------------------------*/
    QScriptValue loadTextFile(QScriptContext *context, QScriptEngine *engine)
    {
        QString contents = _loadTextFile(context,engine);

        return engine->toScriptValue(contents);
    }
/*---------------------------------------------------------------------------------------*/
    QScriptValue loadXMLFile(QScriptContext *context, QScriptEngine *engine)
    {
        if(context->argumentCount() > 0)
        {
            QString templateStr = _loadTextFile(context, engine);
            QString errorStr = "";
            int errorLine = 0;
            int errorColumn = 0;
            doc.clear();
            if(!doc.setContent(templateStr.toUtf8(), false, &errorStr, &errorLine, &errorColumn)){
                qDebug() << errorStr % QString(" at line %1, column %2").arg(errorLine).arg(errorColumn);
                engine->toScriptValue(false);
            }
        }else{
            qDebug() << QString("Wrong number of arguments: %1").arg(context->argumentCount());
        }
        return engine->toScriptValue(true);
    }
/*---------------------------------------------------------------------------------------*/
    QScriptValue parseCode(QScriptContext *context, QScriptEngine *engine){
        if(context->argumentCount() > 1){
            if(context->argument(1).isString())
            {
                QString variableName = context->argument(1).toString();
                QScriptValue templte = loadTextFile(context, engine);
                QString codeFileStr = templte.toString();
                QStringList lines = codeFileStr.split(";");
                foreach(QString line, lines){
                    if(line.contains(variableName,Qt::CaseInsensitive) &&
                       line.contains("=",Qt::CaseInsensitive)){
                       QStringList expression = line.split("=");
                       if(expression.length() > 1){
                           QString retval = expression.at(1).simplified().replace("\"","");
                          //qDebug() << QString("var:%1, val:%2").arg(expression.at(0)).arg(retval);
                          return engine->toScriptValue(retval);
                       }
                    }
                }
            }
        }
        return engine->toScriptValue(QString());
    }
/*---------------------------------------------------------------------------------------*/
        QScriptValue applyTemplate(QScriptContext *context, QScriptEngine *engine)
        {
            if(context->argumentCount() > 1){
                QScriptValue templte = loadTextFile(context, engine);
                QScriptValue obj = context->argument(1);
                if(obj.isValid())
                {
                    QScriptValue data = engine->toObject(obj);
                    QString templateStr = templte.toString();

                    if(data.isObject())
                    {
                        QScriptValueIterator it(data);
                        while (it.hasNext()){
                            it.next();
                            QString markStart =  "<" % it.name() % ">";
                            if(it.value().isString()){
                                templateStr.replace(markStart, it.value().toString());
                            }else  if(it.value().isBool())
                            {
                                QString markEnd   =  "</" % it.name() % ">";
                                QString marker = markStart % "*" % markEnd;
                                //qDebug() << "To replace: " % marker;
                                QRegExp rx(marker,Qt::CaseInsensitive,QRegExp::Wildcard);

                                if(!it.value().toBool()){
                                    templateStr.replace(rx, "");
                                    //qDebug() << "Replaced: " % marker;
                                }else{
                                    templateStr.replace(markStart, "");
                                    templateStr.replace(markEnd, "");
                                }
                            }else  if(it.value().isNumber()){
                                qsreal val = it.value().toNumber();
                                templateStr.replace(markStart,QString::number(val));

                            }else{
                                //qDebug() << QString("Property %1 is '%2', not a string or bool").arg(it.name()).arg(QString(it.value().toVariant().typeName()));
                            }
                        }
                        return engine->toScriptValue(templateStr);
                    }else{
                        qDebug() << "Not an object";
                    }
                }else{
                    qDebug() << "Received object is invalid";
                }
            }else{
                qDebug() <<QString("applyTemplate: wrong number of arguments %1. Required 2").arg(context->argumentCount());
            }
            return engine->toScriptValue(QString());
        }
    /*---------------------------------------------------------------------------------------*/
        QString getArgument(QScriptContext *context,int index){
            QString ret;
            if(context->argumentCount() > index){
                QScriptValue tn = context->argument(index);
                if(tn.isValid() && tn.isString()){
                    ret = tn.toString();
                }
            }else{
                qDebug() << QString("There is no argument with index %1").arg(index);
            }
            return ret;
        }
    /*---------------------------------------------------------------------------------------*/
        QScriptValue getListOfXmlTags(QScriptContext *context, QScriptEngine *engine)
        {
            if(context->argumentCount() > 1)
            {
                loadXMLFile(context, engine);
                if(!doc.isNull())
                {
                    QString tagName = getArgument(context,1);
                    QString atrName;
                    if(context->argumentCount() > 2)
                    {
                       atrName = getArgument(context,2);
                       if(atrName.isEmpty()){
                          qDebug() << "getListOfXmlTags: Attribute name is not specified. Returning an empty array";
                          return engine->toScriptValue(QVariantList());
                       }
                    }
                    if(tagName.isEmpty()){
                       qDebug() << "getListOfXmlTags: Tag name is not specified. Returning an empty array";
                       return engine->toScriptValue(QVariantList());
                    }

                    QVariantList ret;
                    QDomNodeList elements = doc.elementsByTagName(tagName);
                    qDebug() << QString("getListOfXmlTags: looking for %1 tag. Found %2").arg(tagName).arg(elements.count());
                    for(int i = 0;i < elements.count();i++)
                    {
                        QDomNode e = elements.at(i);
                        if(atrName.isEmpty())
                        {
                            QDomElement textNode = e.toElement();
                            ret.append(textNode.text().trimmed());
                        }
                        else
                        {
                            QDomNamedNodeMap attributes = e.attributes();
                            if(attributes.contains(atrName)){
                               QString attrValue = attributes.namedItem(atrName).nodeValue();
                               ret.append(attrValue);
                            }else{
                               qDebug() << QString("getListOfXmlTags: No such attribute %1").arg(atrName);
                            }
                        }
                    }
                    return engine->toScriptValue(ret);
                }
                else
                {
                    qDebug() << "getListOfXmlTags: document is empty";
                    return engine->toScriptValue(QVariantList());
                }
            }
            else
            {
                qDebug() << "getListOfXmlTags: Wrong number of arguments. Need at list 2";
                return engine->toScriptValue(QVariantList());
            }
        }
    /*---------------------------------------------------------------------------------------*/
        QScriptValue applyXMLtemplate(QScriptContext *context, QScriptEngine *engine)
        {
            if(context->argumentCount() > 1)
            {
                loadXMLFile(context, engine);
                if(!doc.isNull())
                {
                    QString encoding = "UTF-16";
                    if(context->argumentCount()>2){
                        QScriptValue enc = context->argument(2);
                        if(enc.isValid()){
                            if(enc.isString()){
                                encoding = enc.toString();
                                qDebug() << QString("Will save xml file with %1 encoding").arg(encoding);
                            }
                        }
                    }

                    QScriptValue obj = context->argument(1);
                    if(obj.isValid())
                    {
                        QScriptValue data = engine->toObject(obj);
                        if(data.isObject())
                        {
                            QScriptValueIterator it(data);
                            while (it.hasNext())
                            {
                                it.next();
                                QString searchId = it.name();
                                QDomElement tag = doc.elementById(searchId);
                                if(!tag.isNull())
                                {
                                    QDomNodeList chld = tag.childNodes();
                                    for(int ch = 0;ch < chld.length();ch++){
                                        tag.removeChild(chld.at(ch));
                                    }
                                    if(it.value().isString()){
                                       //QByteArray ba = it.value().toString().toLocal8Bit();
                                        //QTextCodec *codec = QTextCodec::codecForName(stringToChar(encoding));
                                        //QTextDecoder *dec = codec->makeDecoder();
                                        QString utfString = it.value().toString();// dec->toUnicode(ba);
                                        QDomText text = doc.createTextNode(utfString);
                                        tag.appendChild(text);
                                    }
                                    else if(it.value().isBool())
                                    {
                                        QDomNode paren = tag.parentNode();
                                        if(!it.value().toBool()){
                                          paren.removeChild(tag);
                                        }
                                    }
                                    else  if(it.value().isNumber())
                                    {
                                        qsreal val = it.value().toNumber();
                                        QDomText text = doc.createTextNode(QString::number(val));
                                        tag.appendChild(text);
                                    }else{
                                        //qDebug() << QString("Property %1 is '%2', not a string or bool").arg(it.name()).arg(QString(it.value().toVariant().typeName()));
                                    }
                                }else{
                                    //qDebug() << QString("tag with id='%1' not found").arg(searchId);
                                }
                            }
                        }else{
                            qDebug() << "Not an object";
                        }
                    }else{
                        qDebug() << "Received object is invalid";
                    }
                    bool applySuffix = true;
                    QString suffix = "_tmp";
                    if(context->argumentCount()>=3){
                        QScriptValue enc = context->argument(3);
                        if(enc.isValid()){
                            if(enc.isBool()){
                                applySuffix = enc.toBool();
                                if(!applySuffix){
                                   suffix = "";
                                }
                            }
                        }
                    }
                    QString path = context->argument(0).toString();
                    int li = path.lastIndexOf(QLatin1String("."));
                    QString ext = path.right(path.length() - li);
                    QString saveFile = validatePath(path.left(li), engine) % "_tmp" % ext;
                    QRegExp ex("\n");
                    QString xml(doc.toByteArray());
                    xml.replace(ex,QString(""));
                    QString xml2 = xml.simplified();
                    xml2.replace(QString("> <"),QString("><"));



                    if(applySuffix){
                        bool ok = writeFile(saveFile,xml2,encoding);
                        if(ok){
                            qDebug() << QString("Saved %1").arg(saveFile);
                            return engine->toScriptValue(true);
                        }
                    }else{
                        return engine->toScriptValue(xml2);
                    }

                }else{
                    qDebug() << "Failed to load XML file";
                }
            }else{
                qDebug() <<QString("applyXMLTemplate: wrong number of arguments %1. Required 2").arg(context->argumentCount());
            }
            return engine->toScriptValue(QString());
        }
/*---------------------------------------------------------------------------------------*/

    QScriptValue listDir(QScriptContext *context, QScriptEngine *engine)
    {
        QVariantMap dirs;
        QString path   = validatePath(context->argument(0).toString(),engine);
        bool filesOnly = context->argument(1).toBool();
        QString _fltr  = context->argument(2).toString();
        QString fltr   = _fltr!="undefined"?_fltr:"*";

        QDir dir(path,fltr,QDir::Unsorted);
        if(filesOnly){
            dir.setFilter(FilesOnly);
        }else{
            dir.setFilter(DirsOnly);
        }
        if(dir.exists()){
            QFileInfoList fi = dir.entryInfoList();
            foreach(QFileInfo f, fi){
                dirs.insert(f.fileName(),f.absoluteFilePath());
                //qDebug() << f.fileName();
            }
            return engine->toScriptValue(dirs);
        }else{
            return context->throwError(QString("Dir does not exists: %1").arg(path));
        }
    }

/*---------------------------------------------------------------------------------------*/
    void cleanupDir(const QString &dirPath,
                    const QString &extFilter,
                    QStringList fileList)
    {
        QDir cont(dirPath,extFilter,QDir::Unsorted,AllDirsAndFiles);

        QFileInfoList files = cont.entryInfoList();

        foreach (QFileInfo fileInfo, files){
            QString fname = fileInfo.fileName();
            if(fileList.contains(fname,Qt::CaseInsensitive)){
                //?? ????? ? ????? ????, ??????? ?????? ? ?????? ??????
                qDebug() << "Ok. File is in list: " % fileInfo.absoluteFilePath();
            }else{
                bool ok;
                ok = QFile(fileInfo.absoluteFilePath()).remove();
                if(ok){
                    qDebug() << "Removed extra file " % fileInfo.absoluteFilePath();
                }else{
                    qDebug() << "Failed to remove extra file " % fileInfo.absoluteFilePath();
                }
            }
        }
    }
/*--------------------------------------------------------------------------------------*/
    bool removeDir(const QString &dirName)
    {
        bool result = true;
        QDir dir(dirName);

        if (dir.exists(dirName)){
            Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
                if(info.isDir()){
                    result = removeDir(info.absoluteFilePath());
                }
                else{
                    result = QFile::remove(info.absoluteFilePath());
                }
                if (!result) {
                    return result;
                }
            }
            result = dir.rmdir(dirName);
        }
        return result;
    }
/*----------------------------------------------------------------------------------------*/
    QScriptValue remDir(QScriptContext *context, QScriptEngine *engine)
    {
        bool dirmoved = false;
        QString dirpath = context->argument(0).isString()? context->argument(0).toString():QString();
        if(!dirpath.isEmpty()){
            dirmoved = removeDir(validatePath(dirpath,engine));
            if(dirmoved){
                qDebug() << QString("Dir %1 successfully removed").arg(dirpath);
            }else{
                qDebug() << QString("Failed to remove dir %1").arg(dirpath);
            }
        }
        return engine->toScriptValue(dirmoved);
    }
/*----------------------------------------------------------------------------------------*/
QString minimizeCss(const QString &body)
{
  QString ret = body;
  ret.replace(QRegExp("[a-zA-Z]+#"),   "#" );
  ret.replace(";}", "}");
  ret.replace(QRegExp("([s:]0)(px|pt|%|em)"),"\\1");
  ret.replace(QRegExp("/*[dD]*?*/"),"");
  ret.replace(QRegExp("\t")," ");
  ret.replace(QRegExp("\r"),"");
  ret.replace(QRegExp("\n"),"");
  return ret.simplified();
}
/*----------------------------------------------------------------------------------------*/
    void _copyFile(const QString &source,
                   const QString &destination,
                   bool updateOnly = false)
    {
        QFile f(source);
        bool copied = false;
        QFile newFile(destination);

        bool fileHasChanged = true;
        if(updateOnly && newFile.exists()){
           QString oldone = fileChecksum(source,QCryptographicHash::Sha1);
           QString newone = fileChecksum(destination,QCryptographicHash::Sha1);
           if(oldone == newone){
              fileHasChanged = false;
           }else{
              //qDebug() << "Has changed:" << source;// << oldone << " <-->" << newone;
           }
        }
        if(updateOnly && !fileHasChanged){
            //qDebug() << "File" << source << "has not changed. NO need to copy";
            return;
        }else{
            //qDebug() << "Copy " % source % " to : " % destination;
        }
        if(newFile.exists()){
           copied = newFile.remove();
        }
        //if(f.fileName().endsWith(".css",Qt::CaseInsensitive))
       // {
            //QString css  = _readTextFile(source);
            //QString _css = minimizeCss(css);
            //writeFile(destination,_css,"UTF-8");
        //}
        //else
        //{
            copied = f.copy(destination);
        //}
        if(!copied){
            qDebug() << "Error copyng " + destination;
        }
    }
/*----------------------------------------------------------------------------------------*/
    QString _dirPathFromFilePath(QString filePath){
        return QFileInfo(filePath).dir().absolutePath() + slh;
    }
/*----------------------------------------------------------------------------------------*/
    QScriptValue dirPathFromFilePath(QScriptContext *context, QScriptEngine *engine){
        QString source  = validatePath(context->argument(0).toString(),engine);
        QString dirpath =_dirPathFromFilePath(source);
        return engine->toScriptValue(dirpath);
    }
/*----------------------------------------------------------------------------------------*/
    QScriptValue copyFile(QScriptContext *context, QScriptEngine *engine){
        if(context->argumentCount() >= 2)
        {
            QString source  = validatePath(context->argument(0).toString(),engine);
            QString destin  = validatePath(context->argument(1).toString(),engine);
            bool updateOnly = context->argument(2).toBool();
            QDir di;
            if(di.mkpath(_dirPathFromFilePath(destin))){
                _copyFile(source,destin,updateOnly);
            }else{
                qDebug() << QString("Failed to make dir for %1").arg(destin);
            }
        }
        return engine->toScriptValue(false);
    }
/*----------------------------------------------------------------------------------------*/
    void _copyToDir(const QString &srceDir, // ??????
                   const QString &destDir,  // ????
                   const QString &nFilter,  // ?????????? ??????, ??????? ???? ??????????
                   bool deleteSource,       // ??????? ???. ?????? ????? ??????????? ?????
                   const QString &excludeItemsContaining,
                   const QString &excludeItemsContainTwo) //??????????, ???????? ????? ??? ????? ?????????? ??? ?????????
    {
        QFileInfo dinf(srceDir);
        if(dinf.isDir() && dinf.exists())
        {
           if((excludeItemsContaining.length()> 0 && dinf.baseName().contains(excludeItemsContaining,Qt::CaseInsensitive)) ||
              (excludeItemsContainTwo.length()> 0 && dinf.baseName().contains(excludeItemsContainTwo,Qt::CaseInsensitive))){
              return;
           }

            QDir d(srceDir,"*",QDir::Unsorted,AllDirsAndFiles);

            if(d.count()>0)
            {
                qDebug() << "Make mkpath: " + destDir;
                d.mkpath(destDir);
                QFileInfoList files = d.entryInfoList();
                QStringList filters = nFilter.contains(",")?nFilter.split(","):QStringList() << nFilter;//???? ?????? ???? '*.cpp,*.cxx,*.cc'
                d.setNameFilters(filters);
                foreach (QFileInfo fileInfo, files)
                {
                    if( excludeItemsContaining.length() == 0 ||
                       !fileInfo.baseName().contains(excludeItemsContaining,Qt::CaseInsensitive) ||
                       !fileInfo.baseName().contains(excludeItemsContainTwo,Qt::CaseInsensitive))
                    {
                        QString source = fileInfo.absoluteFilePath();
                        QString destination = QDir::cleanPath(destDir + slh + fileInfo.fileName());
                        if(fileInfo.isDir())
                        {
                            if(!fileInfo.baseName().contains('@')){
                                _copyToDir(source,
                                           destination,
                                           nFilter,false,
                                           excludeItemsContaining,
                                           excludeItemsContainTwo);
                            }else{
                                qDebug() << QString("Dir %1 containg '@' character, skipping").arg(fileInfo.baseName());
                            }

                        }else{
                            _copyFile(source,destination);
                        }
                    }
                }
                if(deleteSource){//??????? ?????? ?????
                    qDebug() << "Rem dir: " + srceDir;
                    bool okRem = removeDir(srceDir);
                    if(!okRem)
                        qDebug() << "Error removing dir " + srceDir;
                }

            }else{
                qDebug() << srceDir % " dir does not contains thumbs";
            }
        }else{
            qDebug() << srceDir + " dir does not exists or is not a dir";
        }
    }
    QScriptValue copyToDir(QScriptContext *context, QScriptEngine *engine)
    {
        if(context->argumentCount() >= 2){
            QString source = validatePath(context->argument(0).toString(),engine);
            QString destin = validatePath(context->argument(1).toString(),engine);
            QString filtrs = context->argumentCount()>2?context->argument(2).isString()?context->argument(2).toString():"":"";
            QString exclude = context->argumentCount()>3?context->argument(3).isString()?context->argument(3).toString():"":"";
            QString excludeTwo = context->argumentCount()>4?context->argument(4).isString()?context->argument(4).toString():"":"";
            _copyToDir(source,
                       destin,
                       filtrs,
                       false,
                       exclude,
                       excludeTwo);
        }
        return engine->toScriptValue(false);
    }
/*--------------------------------------------------------------------------------------*/
    void addSetting(const QString &param, const QVariant &value){
        settings.setValue(param, value);
    }
/*--------------------------------------------------------------------------------------*/
    QVariant getSetting(const QString &param, const QVariant &defval){
        return settings.value(param, defval);
    }
/*--------------------------------------------------------------------------------------*/
    void setGroupSetting(const QString &groupName,
                                     const QString &key,
                                     const QString &value)
    {
        QString clKey = cleanStr(key, false);
        QString _key = groupName % "/" % clKey;
        settings.setValue(_key,value);
    }
/*--------------------------------------------------------------------------------------*/
    void clearGroupSetting(const QString &groupName){
        settings.beginGroup(groupName);
        settings.remove("");
        settings.endGroup();
    }
/*--------------------------------------------------------------------------------------*/
    QVariantMap getGroupSetting(const QString &groupName)
    {
        QVariantMap ret;
        settings.beginGroup(groupName);
        QStringList keys = settings.childKeys();
        foreach(QString key, keys){
            QVariant val = settings.value(key);
            if(QString(val.typeName()) == "QString"){
                ret.insert(key,val.toString());
            }
        }
        settings.endGroup();
        return ret;
    }
/*--------------------------------------------------------------------------------------*/
    QScriptValue fileInfo(QScriptContext *context, QScriptEngine *engine)
    {
        QVariantMap file;
        //if(context->argumentCount() == 1)
        //{
            QString path = validatePath(context->argument(0).toString(),engine);
            QString filepath = validatePath(path, engine);
            QFileInfo fi(filepath);
            QDir d = fi.absoluteDir();
            if(fi.exists()){
                file.insert("basename",        QVariant::fromValue(fi.baseName()));
                file.insert("filename",        QVariant::fromValue(fi.fileName()));
                file.insert("exists",        QVariant::fromValue(true));
                file.insert("absoluteFilePath",QVariant::fromValue(fi.absoluteFilePath()));
                file.insert("completeSuffix",QVariant::fromValue(fi.completeSuffix()));
                file.insert("suffix",        QVariant::fromValue(fi.suffix()));
                file.insert("created",       QVariant::fromValue(fi.birthTime()));
                file.insert("readable",      QVariant::fromValue(fi.isReadable()));
                file.insert("size",          QVariant::fromValue(fi.size()));
                file.insert("lastModified",  QVariant::fromValue(fi.lastModified()));
                file.insert("lastRead",      QVariant::fromValue(fi.lastRead()));
                file.insert("parentDir",      QVariant::fromValue(d.absolutePath()));
                if(fi.suffix() == "exe" || fi.suffix() == "dll"){
                    file.insert("fileVersion","");
                }else{
                    file.insert("fileVersion","");
                }
            }
            else
            {
                //qDebug() << QString("File does not exists: %1").arg(path);
                file.insert("exists",false);
            }
        //}
        return engine->toScriptValue(file);
    }
/*--------------------------------------------------------------------------------------*/
    QScriptValue makePath(QScriptContext *context, QScriptEngine *engine)
    {
        Q_UNUSED(engine);
        if(context->argumentCount() == 1)
        {
            if(context->argument(0).isString() &&
               context->argument(0).isValid()){
                QString _filePath = cleanStr(context->argument(0).toString(),true,true);
                QString filePath = translit(_filePath.toLocal8Bit());
                return engine->toScriptValue(filePath);
            }else{
                return engine->toScriptValue(QString());
            }
        }else{
            return engine->toScriptValue(QString());
        }
    }
/*--------------------------------------------------------------------------------------*/
    QScriptValue save(QScriptContext *context, QScriptEngine *engine)
    {
        Q_UNUSED(engine);
        if(context->argumentCount() == 2)
        {
            if(context->argument(0).isString() &&
               context->argument(0).isValid() &&
               context->argument(1).isValid())
            {
                QString _filePath = cleanStr(context->argument(0).toString(),true,true);
                QString filePath = translit(_filePath.toLocal8Bit());
                QString fileData = "";
                if(context->argument(1).isString())
                {
                    fileData = context->argument(1).toString();
                }
                else if(context->argument(1).isObject())
                {
                    /*QJson::Serializer serializer;
                    serializer.allowSpecialNumbers(false);
                    serializer.setIndentMode(QJson::IndentFull);
                    QByteArray json = serializer.serialize(context->argument(1).toVariant());
                    fileData = QString(json);*/
                }
                QString path = validatePath(filePath, engine);
                //qDebug() << QString("Saving file to %1").arg(path);
                writeFile(path,fileData,"UTF-8");
            }else{
                qDebug() << QString("One of arguments is not a string as it should");
            }
        }else{
            qDebug() << QString("Wrong number of arguments: %1").arg(context->argumentCount());
        }
        return engine->toScriptValue(false);
    }
/*--------------------------------------------------------------------------------------*/
    QScriptValue runProcess(QScriptContext *context, QScriptEngine *engine){
        if(context->argumentCount() > 0)
        {
            QString filePath = context->argument(0).toString();
          //??????? ?????
            /*QString currScriptPath = engine->globalObject().property("qs").property("script").property("absoluteFilePath").toString();
            QFileInfo workFile(currScriptPath);
            QDir workDir(workFile.absoluteFilePath());
            QString workDirPath = workDir.absolutePath() % slh;
            qDebug() << QString("Set working dir to %1").arg(workDirPath);*/

            qDebug() << QString("Process: %1").arg(filePath);

            QProcess runner;
            //runner.setWorkingDirectory(workDirPath);
            QStringList arguments;
            runner.setProcessChannelMode(QProcess::SeparateChannels);//QProcess::SeparateChannels //QProcess::MergedChannels
            runner.start(filePath, arguments, QIODevice::ReadWrite);
            if(!runner.waitForStarted())
                return engine->toScriptValue(false);

            QByteArray data;
            while(runner.waitForReadyRead())
                data.append(runner.readAllStandardOutput());//.readAll()

            qDebug() << data.data();

            runner.waitForFinished(360000);

            return engine->toScriptValue(true);

        }else{
            return engine->toScriptValue(false);
        }
    }
/*---------------------------------------------------------------------------------------*/
    QScriptValue resolvePath(QScriptContext *context, QScriptEngine *engine){
        if(context->argumentCount() > 0){
            QString filePath = context->argument(0).toString();
            return engine->toScriptValue(validatePath(filePath, engine));
        }else{
            return engine->toScriptValue(QString());
        }
    }
    /*---------------------------------------------------------------------------------------*/
    QString _xslProcess(const QString & layoutXmlFilePath,const QString & xslFilePath,const QString & xmlFilePath, const QString & lang){
        QString out;
        QXmlQuery query(QXmlQuery::XSLT20);
        QUrl lay(layoutXmlFilePath);
        //QUrl xml(xmlFilePath);
        QUrl xsl(xslFilePath);

        lay.setScheme("file");
        xsl.setScheme("file");

        query.bindVariable("contentUrl", QVariant(xmlFilePath));
        query.bindVariable("lang", QVariant(lang));
        query.setFocus(lay);
        query.setQuery(xsl);
        query.evaluateTo(&out);

        //qDebug() << out;

        return out;
    }
    /*--------------------------------------------------------------------------------------------------------*/
    QScriptValue xslProcess(QScriptContext *context, QScriptEngine *engine){
        if(context->argumentCount() >= 3)
        {
            //QString layPath = validatePath(context->argument(0).toString(), engine);
            QString layFile = translit(cleanStr(context->argument(0).toString(),true,true).toLocal8Bit());

            //QString xmlPath = validatePath(context->argument(1).toString(), engine);
            QString xslFile = translit(cleanStr(context->argument(1).toString(),true,true).toLocal8Bit());

            //QString xslPath = validatePath(context->argument(2).toString(), engine);
            QString xmlFile = translit(cleanStr(context->argument(2).toString(),true,true).toLocal8Bit());

            QString lang    = context->argumentCount()>3? translit(cleanStr(context->argument(3).toString(),true,true).toLocal8Bit()):QString("en");

            QString layPath = validatePath(layFile, engine);
            QString xmlPath = validatePath(xmlFile, engine);
            QString xslPath = validatePath(xslFile, engine);

            return engine->toScriptValue(_xslProcess(layPath,xslPath,xmlPath,lang));
        }
        else
        {
            return engine->toScriptValue(QString());
        }
    }
    /*---------------------------------------------------------------------------------------*/
    QScriptValue renderSvg(QScriptContext *context, QScriptEngine *engine){
        if(context->argumentCount() > 2)
        {
            QString _path   = context->argument(0).toString();
            QString path    = validatePath(_path, engine);
            QString id      = context->argument(1).toString();
            QString outPath = context->argument(2).toString();
            QFileInfo fi(path);
            if(fi.exists() && fi.isFile())
            {
                QSvgRenderer *renderer = new QSvgRenderer(path);
                QRectF target = renderer->boundsOnElement(id);
                                //renderer->transformForElement(id);


                QGraphicsSvgItem *black = new QGraphicsSvgItem();
                black->setSharedRenderer(renderer);
                black->setElementId(id);
                //QRectF bbox = black->boundingRect();
                renderer->setViewBox(target);


                QImage pic(target.width(),target.height(),QImage::Format_ARGB32);
                pic.fill(Qt::transparent);

                QPainter painter(&pic);
                renderer->render(&painter);
                pic.save(outPath);

                return engine->toScriptValue(true);
            }
            else
            {
                qDebug() << path << " does not exists or is not a file.";
                return engine->toScriptValue(QString());
            }
        }
        else
        {
            qDebug() << "Wrong number of arguments. Need at least two.";
            return engine->toScriptValue(QString());
        }
    }
/*========================================================================================*/
/*
char *fullpath (const char *path)
{
    LPTSTR lpFilePart;
    DWORD nBufferLength = 0;
    LPTSTR lpBuffer = NULL;
    nBufferLength = GetFullPathName (path, 0, lpBuffer, &lpFilePart);
    if (!nBufferLength)
        return path;
    lpBuffer = (LPTSTR) malloc (nBufferLength + 1);
    if (!lpBuffer)
        return path;
    if (GetFullPathName (path, nBufferLength, lpBuffer, &lpFilePart))
        return lpBuffer;
    else {
        free (lpBuffer);
        return path;
    }
}
char *searchpath (const char *path, const char *ext)
{
    LPTSTR lpFilePart, lpExt = NULL;
    DWORD nBufferLength = 0;
    LPTSTR lpBuffer = NULL;
    if (!strchr (path, '.'))
        lpExt = ext;
    nBufferLength = SearchPath (NULL, path, lpExt, 0, lpBuffer, &lpFilePart);
    if (!nBufferLength)
        return path;
    lpBuffer = (LPTSTR) malloc (nBufferLength + 1);
    if (!lpBuffer)
        return path;
    if (SearchPath (NULL, path, lpExt, nBufferLength, lpBuffer, &lpFilePart))
        return lpBuffer;
    else {
        free (lpBuffer);
        return path;
    }
}*/
}
