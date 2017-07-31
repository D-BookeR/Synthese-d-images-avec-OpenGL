#!/bin/bash

## compile et lance le projet

# met un lien sur les dossiers gradle et .gradle
if test ! -L gradle
then
    ln -sr ../../../common/Android/gradle gradle
fi

# dossier temporaire
mkdir -p app/src/debug/java

# compiler le projet
./gradlew assembleDebug || exit 1

# dossier temporaire
rm -fr app/src/debug

# configuration
export PATH=$PATH:/usr/local/android-sdk/platform-tools:/usr/local/android-sdk/tools

# lancer l'émulateur
if ! pidof -s /usr/local/android-sdk/tools/emulator64-crash-service
then
    /usr/local/android-sdk/tools/emulator -avd AVD24 &
    sleep 15
fi

# installer l'APK
adb install -r app/build/outputs/apk/app-debug.apk || exit 1

# lancer l'activité
adb shell am start -n com.dbooker.livreopengl/com.dbooker.livreopengl.OpenGLActivity
