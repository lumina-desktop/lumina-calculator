//===========================================
//  Lumina Desktop source code
//  Copyright (c) 2016, Ken Moore
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QTranslator>
#include <QFile>
#include <QTextCodec>

#include "mainUI.h"

QString PathToAbsolute(QString path){
  //Convert an input path to an absolute path (this does not check existance ot anything)
  if(path.startsWith("/")){ return path; } //already an absolute path
  if(path.startsWith("~")){ path.replace(0,1,QDir::homePath()); }
  if(!path.startsWith("/")){
    //Must be a relative path
    if(path.startsWith("./")){ path = path.remove(2); }
    path.prepend( QDir::currentPath()+"/");
  }
  return path;
}

QTranslator* LoadTranslation(QApplication *app, QString appname, QString locale = "", QTranslator *cTrans = 0){
   //Get the current localization
    QString langEnc = "UTF-8"; //default value
    QString langCode = locale; //provided locale
    if(langCode.isEmpty()){ langCode = getenv("LC_ALL"); }
    if(langCode.isEmpty()){ langCode = getenv("LANG"); }
    if(langCode.isEmpty()){ langCode = "en_US.UTF-8"; } //default to US english
    //See if the encoding is included and strip it out as necessary
    if(langCode.contains(".")){
      langEnc = langCode.section(".",-1);
      langCode = langCode.section(".",0,0);
    }
    //Now verify the encoding for the locale
    if(langCode =="C" || langCode=="POSIX" || langCode.isEmpty()){
      langEnc = "System"; //use the Qt system encoding
    }
    if(app !=0){
      qDebug() << "Loading Locale:" << appname << langCode << langEnc;
      //If an existing translator was provided, remove it first (will be replaced)
      if(cTrans!=0){ app->removeTranslator(cTrans); }
      //Setup the translator
      cTrans = new QTranslator();
      //Use the shortened locale code if specific code does not have a corresponding file
      if(!QFile::exists(L_SHAREDIR+"/lumina-calculator/i18n/"+appname+"_" + langCode + ".qm") && langCode!="en_US" ){
        langCode.truncate( langCode.indexOf("_") );
      }
      QString filename = appname+"_"+langCode+".qm";
      //qDebug() << "FileName:" << filename << "Dir:" << LOS::LuminaShare()+"i18n/";
      if( cTrans->load( filename, L_SHAREDIR+"/lumina-calculator/i18n/" ) ){
        app->installTranslator( cTrans );
      }else{
	//Translator could not be loaded for some reason
	cTrans = 0;
	if(langCode!="en_US"){
	  qWarning() << " - Could not load Locale:" << langCode;
	}
      }
    }else{
      //Only going to set the encoding since no application given
      qDebug() << "Loading System Encoding:" << langEnc;
    }
    //Load current encoding for this locale
    QTextCodec::setCodecForLocale( QTextCodec::codecForName(langEnc.toUtf8()) );
    return cTrans;
}

int  main(int argc, char *argv[]) {

   QApplication a(argc, argv);
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
   LoadTranslation(&a, "l-calc");
   //Now start the window
   mainUI W;
   W.show();
   return  a.exec();
}
