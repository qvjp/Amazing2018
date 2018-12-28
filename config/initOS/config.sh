#!/bin/bash

E_ERROR=65    # 安装失败
E_OK=0
ROOT_UID=0

if [ "$UID"  != "$ROOT_UID" ]
then
    echo "Only root can run this script!"
    exit $E_ERROR
fi

CURRENT=`pwd`

# 任务类型
I_NSTALL="Install"   
U_PDATE="Update"
D_OWNLOAD="Download"

# 输出日志
LOG1=$CURRENT/log.log
LOG2=$CURRENT/err.log

# 必要工具
NEC_TOOLS="wget cloc vim git zsh tree icdiff"

# 常用项目
PROJECTS="INLOW Amazing2018 Playground"

# Git 配置所用邮箱
UEMAIL="qvjunping@gmail.com"
UNAME="Qv Junping"

# Operating System
OS=DARWIN

# Linux Distro
DistroBasedOn="RadHat"
PKGINS="yum"

# 每次执行此脚本都会清空原来的日志文件
echo "" > $LOG1 > $LOG2

# -p下载常用项目
if [ "$1" == "-p" ]; then
    DOWNPRO=DOWNPRO
fi

# Param0 $?
# Param1 程序名
# Param2 程序类型
CheckStatus()
{
    if [ "$1" == 0 ]
    then
        echo "$2 $3 Success." 
    else
        echo "$2 $3 Fail."
        exit $E_ERROR
    fi
}

InitDarwin()
{
    # 连接Github
    ssh-keygen -t rsa -b 4096 -C "qvjunping@gmail.com"
    eval "$(ssh-agent -s)"
    echo -e "Host *\n AddKeysToAgent yes\n UseKeychain yes\n IdentityFile ~/.ssh/id_rsa" > ~/.ssh/config
    ssh-add -K ~/.ssh/id_rsa
    clip < ~/.ssh/id_rsa.pub
}

InitLinux()
{
    if [ "$DistroBasedOn" = "Debian" ]; then
        PKGINS="apt-get"
    elif [ "$DistroBasedOn" = "RedHat" ]; then
        PKGINS="yum"
    elif [" $DistroBasedOn" = "SuSe" ]; then
        PKGINS="zypper"
    fi
    # 必要工具
    echo "Initializing..."
    echo; echo "Installding necessary tools"
    echo "Installing $NEC_TOOLS"
    $PKGINS install $NEC_TOOLS -y 1>>$LOG1 2>>$LOG2
    CheckStatus "$?" "yum install $NEC_TOOLS" "$I_NSTALL"
    echo "Necessary tools install is complete!"; echo
}

InitCommon()
{
    echo; echo "Configuring necessary tools"
    # 配置zsh，并将zsh作为默认shell
    echo "Configuring zsh"
    if [ ! -f ~/.zshrc ]; then
        wget -c https://raw.githubusercontent.com/qvjp/Amazing2018/master/config/zsh/zshrc -O ~/.zshrc 1>>$LOG1 2>>$LOG2
        CheckStatus "$?" "zshrc" "$D_OWNLOAD"
    fi
    chsh -s /bin/zsh 1>>$LOG1 2>>$LOG2
    CheckStatus "$?" "chsh -s /bin/zsh" "$U_PDATE"

    # 配置vim
    echo "Configuring vim"
    if [ ! -f ~/.vimrc ]; then
        wget -c https://raw.githubusercontent.com/qvjp/Amazing2018/master/config/vim/vimrc -O ~/.vimrc 1>>$LOG1 2>>$LOG2
        CheckStatus "$?" "vimrc" "$D_OWNLOAD"
    fi
    # vim插件目录
    DIR_VIM_PLUGINS=~/.vim/pack/git-plugins/start
    if [ ! -d $DIR_VIM_PLUGINS ]; then
        mkdir -p "$DIR_VIM_PLUGINS" 2>>$LOG2
        git clone https://github.com/w0rp/ale.git ~/.vim/pack/git-plugins/start/ale 1>>$LOG1 2>>$LOG2
        CheckStatus "$?" "ale" "$D_OWNLOAD"
    fi

    # 配置git
    echo "Configuring git"
    git config --global user.email "$UEMAIL"
    git config --global user.name "$UNAME"

    echo "Necessary tools configuration is complete!"; echo

}

DownloadProjects()
{
    echo; echo "Downloading frequently used projects"
    mkdir -p ~/Programming
    for pro in $PROJECTS
    do
        if [ ! -d ~/Programming/$pro ]; then
            git clone git@github.com:qvjp/$pro.git ~/Programming/$pro 1>>$LOG1 2>>$LOG2
            CheckStatus "$?" "$pro" "$D_OWNLOAD"
        fi
    done

    echo "Frequently used projects download is complete!, go ~/Programming find them."; echo
}

LastWords()
{
    echo "最后一步，连接到Github"
    # 连接Github
    ssh-keygen -t rsa -b 4096 -C "$UEMAIL"
    eval "$(ssh-agent -s)"
    ssh-add ~/.ssh/id_rsa

    echo
    echo "已全部安装配置完成"
    echo
    echo "========================================================="
    echo "记得将~/.ssh/id_rsa.pub粘贴到Github，macOS用户可直接粘贴"
    echo "========================================================="
    echo
}

# 下载常用项目
if [ ! -z "$DOWNPRO" ]; then
    DownloadProjects
    exit $E_OK
fi

# 支持macOS, Linux
case "$OSTYPE" in
    linux* )
        if [ -f /etc/redhat-release ]; then
            DistroBasedOn="RedHat"
        elif [ -f /etc/debian_version ]; then
            DistroBasedOn="Debian"
        elif [ -f /etc/SuSE-release ]; then
            DistroBasedOn="SuSe"
        fi
        echo "Your Operating system is $DistroBasedOn Based Linux"
        InitLinux
        InitCommon
        OS=LINUX;;
    Darwin*)
        echo "Your Operating system is Darwin"
        InitDarwin
        InitCommon
        OS=DARWIN;;
    *)
        echo "Unknown: $OSTYPE"
        exit $E_ERROR
esac

LastWords

