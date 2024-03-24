#include <QFile>
#include <QDebug>
#include <QProcessEnvironment>
#include <QCoreApplication>
#include <QScriptEngine>
#include <QScriptValue>
#include "Classes.h"

namespace
{
    bool loadFile(QString fileName, QScriptEngine *engine)
    {
        // avoid loading files more than once
        static QSet<QString> loadedFiles;
        QFileInfo fileInfo(fileName);
        QString absoluteFileName = fileInfo.absoluteFilePath();
        QString absolutePath = fileInfo.absolutePath();
        QString canonicalFileName = fileInfo.canonicalFilePath();

        if (loadedFiles.contains(canonicalFileName)) {
            return true;
        }

        loadedFiles.insert(canonicalFileName);
        //QString path = fileInfo.path();

        //......???????? ?????..............................................................
        QFile file(fileName);
        if (file.open(QFile::ReadOnly))
        {
            QTextCodec *tc = QTextCodec::codecForName("UTF-8");
            QTextStream stream(&file);
            stream.setCodec(tc);
            QString contents = stream.readAll();
            file.close();

            int endlineIndex = contents.indexOf('\n');
            QString line = contents.left(endlineIndex);
            int lineNumber = 1;

            // strip off #!/usr/bin/env qscript line
            if (line.startsWith("#!")) {
                contents.remove(0, endlineIndex+1);
                ++lineNumber;
            }
            QScriptSyntaxCheckResult check = QScriptEngine::checkSyntax(contents);
            if(check.state() == QScriptSyntaxCheckResult::Error)
            {
                int errClmn = check.errorColumnNumber();
                int errLine = check.errorLineNumber();
                QString errMsge = check.errorMessage();
                qDebug() << QString("Script error. Line: %1, col: %2\n%3").arg(errLine).arg(errClmn).arg(errMsge);
                return false;
            }
            else if(check.state() == QScriptSyntaxCheckResult::Intermediate)
            {
                int errClmn = check.errorColumnNumber();
                int errLine = check.errorLineNumber();
                QString errMsge = check.errorMessage();
                qDebug() << QString("Script error. Line: %1, col: %2\n%3").arg(errLine).arg(errClmn).arg(errMsge);
                return false;
            }
            else if(check.state() == QScriptSyntaxCheckResult::Valid)
            {
                //qDebug() << QString("Script %1 is valid").arg(fileName);
                // set qt.script.absoluteFilePath
                QScriptValue script = engine->globalObject().property("qs").property("script");
                QScriptValue oldFilePathValue = script.property("absoluteFilePath");
                QScriptValue oldPathValue = script.property("absolutePath");

                script.setProperty("absoluteFilePath", engine->toScriptValue(absoluteFileName));
                script.setProperty("absolutePath", engine->toScriptValue(absolutePath));

                QScriptContext *context = engine->currentContext();
                QScriptContext *parent = context->parentContext();

                if(parent!=0){
                   context->setActivationObject(context->parentContext()->activationObject());
                   context->setThisObject(context->parentContext()->thisObject());
                }

                QScriptValue r = engine->evaluate(contents, fileName, lineNumber);
                if (engine->hasUncaughtException()) {
                    QStringList backtrace = engine->uncaughtExceptionBacktrace();
                    qDebug() << QString("    %1\n%2\n\n").arg(r.toString()).arg(backtrace.join("\n"));
                    return true;
                }
                script.setProperty("absoluteFilePath", oldFilePathValue); // if we come from includeScript(), or whereever
                script.setProperty("absolutePath", oldPathValue); // if we come from includeScript(), or whereever
            }

        } else {
            return false;
        }
        return true;
    }
/*---------------------------------------------------------------------------------------*/
    QScriptValue includeScript(QScriptContext *context, QScriptEngine *engine)
    {
        QString currentFileName = engine->globalObject().property("qs").property("script").property("absoluteFilePath").toString();
        QFileInfo currentFileInfo(currentFileName);
        QString path = currentFileInfo.path();
        QString importFile = context->argument(0).toString();
        QFileInfo importInfo(importFile);
        if (importInfo.isRelative()) {
            importFile =  path + "/" + importInfo.filePath();
        }
        if (!loadFile(importFile, engine)) {
            return context->throwError(QString("Failed to resolve include: %1").arg(importFile));
        }
        return engine->toScriptValue(true);
    }
/*---------------------------------------------------------------------------------------*/
    QScriptValue consoleLog(QScriptContext *context, QScriptEngine *engine)
    {
        QString logString = context->argument(0).toString();
        qDebug() << logString;
        return engine->toScriptValue(true);
    }
}
/*---------------------------------------------------------------------------------------*/

/*=======================================================================================*/
int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();
    QScriptEngine *engine = new QScriptEngine();

    if(args.count() >= 2)
    {
        QString fileName = args.at(1);

        QVariantMap vargs;
        if(args.length()>=3){
            for(int a=2;a < args.length();a++){
                QString argStr = args.at(a);
                if(argStr.contains("=")){
                    Qt::SplitBehavior behavior = Qt::KeepEmptyParts;
                    QStringList sl = argStr.split("=",behavior);
                    if(sl.length()>=2){
                        vargs.insert(sl.at(0),sl.at(1));
                    }
                }
            }
        }

        engine->importExtension("qt.core");
        //engine->importExtension("qt.gui");

//---------------------------------------------------------------------------------
        QScriptValue global = engine->globalObject();
        global.setProperty("qs", engine->newObject());

        QScriptValue script = engine->newObject();
        global.property("qs").setProperty("script", script);

        QScriptValue picture = engine->newObject();
        global.setProperty("picture", picture);
        global.property("picture").setProperty("convert",   engine->newFunction(anima::processImage));
        global.property("picture").setProperty("getColor",engine->newFunction(anima::imageColor));

        QScriptValue parser = engine->newObject();
        global.setProperty("parser", parser);
        global.property("parser").setProperty("applyTemplate",   engine->newFunction(anima::applyTemplate));
        global.property("parser").setProperty("applyXMLTemplate",engine->newFunction(anima::applyXMLtemplate));
        global.property("parser").setProperty("parseCode",engine->newFunction(anima::parseCode));
        global.property("parser").setProperty("getListOfXmlTags",engine->newFunction(anima::getListOfXmlTags));

        QScriptValue system = engine->newObject();
        system.setProperty("runProcess", engine->newFunction(anima::runProcess));
        system.setProperty("resolvePath",engine->newFunction(anima::resolvePath));
        global.property("qs").setProperty("system", system);

        QScriptValue log = engine->newFunction(consoleLog);
        global.setProperty("console", engine->newObject());
        global.property("console").setProperty("log", log);

        for(QVariantMap::const_iterator iter = vargs.begin(); iter != vargs.end(); ++iter) {
           //qDebug() << iter.key() << iter.value();
           QVariant val = iter.value();
           if(QString(val.typeName()) == QString("QString")){
                global.setProperty(QString(iter.key()),val.toString());
           }
        }

//---------------------------------------------------------------------------------

    #ifdef Q_OS_WIN32
        QScriptValue osName = engine->toScriptValue(QString("windows"));
    #elif defined(Q_OS_LINUX)
        QScriptValue osName = engine->toScriptValue(QString("linux"));
    #elif defined(Q_OS_MAC)
        QScriptValue osName = engine->toScriptValue(QString("mac"));
    #elif defined(Q_OS_UNIX)
        QScriptValue osName = engine->toScriptValue(QString("unix"));
    #endif
        system.setProperty("os", osName);

    // ?????????? ????? ? qs.system.env ------------------------------------------------
        QMap<QString,QVariant> envMap;
        QProcessEnvironment se = QProcessEnvironment::systemEnvironment();
        QStringList envList = se.toStringList();
        foreach (const QString &entry, envList) {
            QStringList keyVal = entry.split('=');
            if (keyVal.size() == 1)
                envMap.insert(keyVal.at(0), QString());
            else
                envMap.insert(keyVal.at(0), keyVal.at(1));
        }
        system.setProperty("env", engine->toScriptValue(envMap));

    //---???????? include ? qs.script.include ---------------------------------------------
        script.setProperty("include", engine->newFunction(includeScript));
    //-----------------------------------------------------------------------------------

        global.setProperty("btoa", engine->newFunction(anima::btoa));
        global.setProperty("listDir", engine->newFunction(anima::listDir));
        global.setProperty("loadTextFile", engine->newFunction(anima::loadTextFile));
        global.setProperty("fileInfo", engine->newFunction(anima::fileInfo));
        global.setProperty("makePath", engine->newFunction(anima::makePath));
        global.setProperty("save", engine->newFunction(anima::save));
        global.setProperty("copyFile",engine->newFunction(anima::copyFile));
        global.setProperty("copyToDir",engine->newFunction(anima::copyToDir));
        global.setProperty("dirPathFromFilePath",engine->newFunction(anima::dirPathFromFilePath));
        global.setProperty("remDir", engine->newFunction(anima::remDir));
        global.setProperty("getSumForFile", engine->newFunction(anima::getSumForFile));
        global.setProperty("getFileVer", engine->newFunction(anima::getFileVer));
        global.setProperty("xslProcess", engine->newFunction(anima::xslProcess));
        global.setProperty("renderSvg", engine->newFunction(anima::renderSvg));


    //--????????? ??? ??????? ------------------------------------------------------------
        if (!loadFile(args.at(1), engine)) {
            qDebug() << "Failed:" << fileName;
            return EXIT_FAILURE;
        }
        delete engine;
    }else{
        delete engine;
        return EXIT_SUCCESS;
    }
}
