#!/bin/bash

E_ERROR=65    # 安装失败
E_OK=0
ROOT_UID=0

if [ "$UID"  == "$ROOT_UID" ]
then
    echo "Only normal user can run this script!"
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
fifolog=log.fifo
fifoerr=err.fifo
mkfifo $fifolog
mkfifo $fifoerr
cat $fifolog | tee $LOG1 &
cat $fifoerr | tee $LOG2 &
exec 1>$fifolog
exec 2>$fifoerr

# 必要工具
NEC_TOOLS="wget cloc vim git zsh tree icdiff trash"

# 常用项目
PROJECTS="INLOW Amazing2018 Playground TODO"

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
    for tool in $NEC_TOOLS; do
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
    echo "Installing $NEC_TOOLS"
    $PKGINS install $NEC_TOOLS -y
    CheckStatus "$?" "yum install $NEC_TOOLS" "$I_NSTALL"
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
    if [ ! -d ~/.SpaceVim ]; then
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
            git clone git@github.com:qvjp/$pro.git ~/Programming/$pro
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

    if [ "$OS" == LINUE ]; then
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
    echo "已全部安装配置完成"
    echo
    echo "========================================================="
    echo "记得将~/.ssh/id_rsa.pub粘贴到Github，macOS用户可直接粘贴"
    echo "========================================================="
    echo
}


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
    darwin*)
        echo "Your Operating system is Darwin"
        InitDarwin
        InitCommon
        OS=DARWIN;;
    *)
        echo "Unknown: $OSTYPE"
        exit $E_ERROR
esac


while getopts "pg" opt; do
    case $opt in
        p)
            DownloadProjects
        ;;
        g)
            ConnectGithub
        ;;
    esac
done

/bin/rm -rf $fifolog $fifoerr
