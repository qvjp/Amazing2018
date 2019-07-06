#!/bin/bash

# 必要工具
NEC_TOOLS="wget cloc vim git zsh tree icdiff"
NEC_TOOLS_LINUX=${NEC_TOOLS}" nload"
NEC_TOOLS_MAC=${NEC_TOOLS}" trash"

# 常用项目
PROJECTS="INLOW Amazing2018 Playground"
GITHUBNAME="qvjp"

# Git 配置所用邮箱
UEMAIL="qvjunping@gmail.com"
UNAME="Qv Junping"

E_ERROR=65    # 安装失败
E_OK=0
ROOT_UID=0

Exit()
{
    /bin/rm -rf $fifolog $fifoerr
    if [ "$1" == "$E_OK" ]; then
        exit $E_OK;
    fi
    exit $E_ERROR
}

if [ "$UID"  == "$ROOT_UID" ]
then
    echo "Only normal user can run this script!"
    Exit
fi

CURRENT=`pwd`

# 任务类型
I_NSTALL="Install"   
U_PDATE="Update"
D_OWNLOAD="Download"

# 输出日志
LOG1=$CURRENT/log.log
LOG2=$CURRENT/err.log
fifolog=log.fifo
fifoerr=err.fifo
mkfifo $fifolog
mkfifo $fifoerr
cat $fifolog | tee $LOG1 &
cat $fifoerr | tee $LOG2 &
exec 1>$fifolog
exec 2>$fifoerr


# Operating System
OS=DARWIN

# Linux Distro
DistroBasedOn="RadHat"
PKGINS="yum"

# 每次执行此脚本都会清空原来的日志文件
echo "" > $LOG1 > $LOG2


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
        Exit
    fi
}

InitDarwin()
{
    echo "init your Mac..."

    softwareupdate -i -a
    CheckStatus "$?" "softwareupdate" "$U_PDATE"

    #xcode-select --install
    CheckStatus "$?" "xcode-select" "$I_NSTALL"

    echo "Checking brew..."
    if [ ! `which brew` ]; then
        echo "Downloading brew..."
        /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
        CheckStatus "$?" "brew" "$I_NSTALL"
    fi

    brew update
    brew upgrade

    echo "Using brew doweload necessary tools "
    for tool in $NEC_TOOLS_MAC; do
        echo Checking $tool...
        if [ ! `which $tool` ]; then
            echo Downloading $tool
            brew install $tool
            CheckStatus "$?" "brew install $tool" "$I_NSTALL"
        fi
    done
    echo "Necessary tools install is complete!"

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
    echo "Installing $NEC_TOOLS_LINUX"
    for tool in $NEC_TOOLS_LINUX; do
        echo Checking $tool...
        if [ ! `which $tool` ]; then
            echo Downloading $tool
            sudo $PKGINS install $tool -y
            CheckStatus "$?" "$PKGINS install $tool" "$I_NSTALL"
        fi
    done
    echo "Necessary tools install is complete!"; echo
}

InitCommon()
{
    echo =============================
    echo "Configuring necessary tools"
    echo =============================

    # 配置zsh，并将zsh作为默认shell
    echo "Checking zshrc..."
    if [ ! -f ~/.zshrc ]; then
        echo "Downloading zshrc..."
        wget -c https://raw.githubusercontent.com/qvjp/Amazing2018/master/config/zsh/zshrc -O ~/.zshrc
        CheckStatus "$?" "zshrc" "$D_OWNLOAD"
    fi

    echo Checking Defalut Shell...
    if [ ! $SHELL == /bin/zsh ]; then
        chsh -s /bin/zsh
        CheckStatus "$?" "chsh -s /bin/zsh" "$U_PDATE"
    fi

    # 配置vim
    echo "Checking SpaceVim..."
    if [  -d ~/.SpaceVim ]; then
        echo "Downloading SpaceVim..."
        curl -sLf https://spacevim.org/cn/install.sh | bash
        CheckStatus "$?" "SpaceVim" "$D_OWNLOAD"
    fi

    # 配置git
    echo "Configuring global git..."
    git config --global user.email "$UEMAIL"
    git config --global user.name "$UNAME"

    echo ============================================
    echo "Necessary tools configuration is complete!"
    echo ============================================

}

# 支持macOS, Linux
work()
{
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
        darwin*)
            echo "Your Operating system is Darwin"
            InitDarwin
            InitCommon
            OS=DARWIN;;
        *)
            echo "Unknown: $OSTYPE"
            Exit
    esac
}
DownloadProjects()
{
    echo
    echo ============================================
    echo "Downloading frequently used projects"
    echo ============================================
    mkdir -p ~/Programming
    for pro in $PROJECTS
    do
        echo Checking $pro...
        if [ ! -d ~/Programming/$pro ]; then
            git clone git@github.com:$GITHUBNAME/$pro.git ~/Programming/$pro
            CheckStatus "$?" "$pro" "$D_OWNLOAD"
        fi
    done

    echo =============================================================================
    echo "Frequently used projects download is complete!, go ~/Programming find them."
    echo =============================================================================
    echo
}

ConnectGithub()
{
    # 连接Github
    echo "最后一步，连接到Github"

    if [ "$OS" == LINUX ]; then
        ssh-keygen -t rsa -b 4096 -C "$UEMAIL"
        eval "$(ssh-agent -s)"
        ssh-add ~/.ssh/id_rsa
    fi

    if [ "$OS" == DARWIN ]; then
        # 连接Github
        ssh-keygen -t rsa -b 4096 -C "$UEMAIL"
        eval "$(ssh-agent -s)"
        echo -e "Host *\n AddKeysToAgent yes\n UseKeychain yes\n IdentityFile ~/.ssh/id_rsa" > ~/.ssh/config
        ssh-add -K ~/.ssh/id_rsa
        pbcopy < ~/.ssh/id_rsa.pub
    fi

    echo
    echo
    echo "========================================================="
    echo "记得将~/.ssh/id_rsa.pub粘贴到Github，macOS用户可直接粘贴"
    echo "========================================================="
    echo
}

help(){
    echo "Usage:
    `basename $0` options    选择要初始化的功能

Options:
    -o, only          初始化配置电脑
    -p, project       下载已配置项目
    -g, github        连接你的Github

先打开本文件，在文件头配置自已要安装的软件、要clone的Github项目。
macOS、Linux（Debain、Redhat及衍生版）可用。

macOS:
    更新系统自带软件
    安装xcode-select
    安装Homebrew
    用Homebrew安装自已配置列表中的软件

Linux:
    使用系统自带包管理器安装配置列表软件

公共:
    安装并配置zsh，最终作为默认shell
    安装vim，并使用SpaceVim
    配置本地git
    "
    Exit
}


if [ $# -lt 1 ]; then
    help
fi

while getopts "opg" opt; do
    case $opt in
        p)
            work
            DownloadProjects
            ;;
        g)
            work
            ConnectGithub
            ;;
        o)
            work
            ;;
        *)
            help
            ;;
    esac
done

echo "已全部安装配置完成"

Exit $E_OK
