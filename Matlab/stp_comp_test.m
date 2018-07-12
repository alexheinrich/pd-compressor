#!/bin/bash
## Installer for JUCE (https://www.juce.com/)

read -p "Install Dev tools & Dependencies? [Y/n]: " inst
if [ $inst = y ] || [ $inst = Y ]
then

apt-get install build-essential python-dev git npm gnome-tweak-tool openjdk-8-jdk

apt-get install --no-install-recommends gnome-panel

apt-get -y install libwebkit2gtk-4.0 libgtk-3-dev

apt-get install libwebkit2gtk-4.0-dev libwebkit2gtk-4.0-doc

wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
sudo apt-add-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-5.0 main"
sudo apt-get update
sudo apt-get install -y clang-5.0

update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-3.8 100
update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-5.0 1000
update-alternatives --install /usr/bin/clang++ clang /usr/bin/clang-3.8 100
update-alternatives --install /usr/bin/clang clang /usr/bin/clang-3.8 100
update-alternatives --install /usr/bin/clang clang /usr/bin/clang-5.0 1000

## These may not be needed.. But remember to use CLang 5.0 or greated, folks.
 update-alternatives --config clang
 update-alternatives --config clang++

fi

## Installer for JUCE (https://www.juce.com/)
read -p "Install JUCE Huckleberry? [Y/n]: " inst
if [ $inst = y ] || [ $inst = Y ]
then
    cd ~ 
    if [ -e "JUCE/Projects" ]
    then
        mv JUCE/Projects/ /tmp/tmpJUCEProjects/
    fi
    if [ -e "JUCE" ]
    then
        rm -rf JUCE
    fi

    ## https://forum.juce.com/t/freeze-when-opening-filechooser/16026/38
    echo "Cloning JUCE Huckleberry ..."
    git clone -b develop https://github.com/WeAreROLI/JUCE

    ## Install dependencies for JUCE: (https://forum.juce.com/t/juce-4-2-1-setup-on-apt-based-linux-ubuntu-16-04-lts-mint-elementary-os-freya/17164)
    sudo apt-get -q update
    sudo apt-get -y --allow-unauthenticated install llvm clang-5.0 libfreetype6-dev libx11-dev libxinerama-dev libxrandr-dev libxcursor-dev mesa-common-dev libasound2-dev freeglut3-dev libxcomposite-dev libcurl4-gnutls-dev
    
    ## change the default compiler path
    export CC=/usr/bin/clang
    export CXX=/usr/bin/clang++

    ## Create a 'Projects' folder or put the old one back
    if [ -e "/tmp/tmpJUCEProjects" ]
    then
        mv /tmp/tmpJUCEProjects/ JUCE/Projects/
    else
        mkdir JUCE/Projects
    fi

    ## Build the Projucer from Source code
    cd JUCE/extras/Projucer/Builds/LinuxMakefile
    make CONFIG=Release

    if [ ! $? -eq 0 ]
    then
        echo "Building the Projector failed."
        exit
    fi

    mv build/Projucer ~/JUCE/

    ## Create a Desktop launcher
    sudo touch /usr/share/applications/projucer.desktop
    echo "[Desktop Entry]
Name=Projucer
Comment=JUCE Huckleberry Projucer
Exec=$HOME/JUCE/Projucer
Icon=$HOME/JUCE/extras/Projucer/Source/BinaryData/Icons/juce_icon.png
Terminal=false
Type=Application
Categories=AudioVideo;Audio;Graphics;Development;IDE;ProjectManagement;" | sudo tee /usr/share/applications/projucer.desktop | sudo tee $HOME/Desktop/projucer.desktop
    sudo chmod a+x /usr/share/applications/projucer.desktop
    sudo chmod a+x $HOME/Desktop/projucer.desktop

    ## Add the MIME type to the system, so .jucer files will automatically open with the Projucer
    sudo touch /usr/share/mime/packages/juce.xml
    echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>
    <mime-info xmlns=\"http://www.freedesktop.org/standards/shared-mime-info\">
        <mime-type type=\"application/x-juce\">
            <comment>JUCE project</comment>
            <sub-class-of type=\"application/xml\"/>
            <glob pattern=\"*.jucer\"/>
        </mime-type>
    </mime-info>" | sudo tee /usr/share/mime/packages/juce.xml
    sudo update-mime-database /usr/share/mime
    echo "application/x-juce=projucer.desktop" | sudo tee --append /usr/share/applications/defaults.list
    echo "Installed"
fi

## Steinberg VST SDK 3
read -p "Download Steinberg VST SDK 3? [Y/n]: " inst
if [ $inst = y ] || [ $inst = Y ]
then
    cd /tmp/
    if [ -e vstsdk368_08_11_2017_build_121.zip ]
    then
        rm vstsdk368_08_11_2017_build_121.zip
    fi
    echo "Downloading Steinberg VST3 SDK ..."
    wget https://download.steinberg.net/sdk_downloads/vstsdk368_08_11_2017_build_121.zip
    echo "Extracting ..."
    unzip -q vstsdk368_08_11_2017_build_121.zip
    if [ ! -e "$HOME/SDKs" ]
    then
        mkdir "$HOME/SDKs"
    fi

    mv "VST_SDK" "$HOME/SDKs"

    echo "Downloaded."
fi
read -p "Run Projucer? [Y/n]: " inst
if [ $inst = y ] || [ $inst = Y ]
then
    $HOME/JUCE/Projucer
fi