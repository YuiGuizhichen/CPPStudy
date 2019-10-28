# CPPStudy
a tutorial from Cherno.

See here
https://www.notion.so/tsutsukakushitsuki/How-the-C-Linker-Works-8c4ee95ad96747fdbe817f31e21cb31b

# How the C++ Linker Works

Linking 是从 c++ 源码到可执行二进制时的一个过程，编译见上一篇文章。在编译完成后，每个编译单元都是独立存在的 obj，他们之间无法沟通。Linker 就承担了将这些文件链接到一个程序的作用。

即使你把整个程序都在写一个文件里，应用程序也需要知道 Entry Point 在哪。大部分时候，Entry Point 都是 main 方法，这也就是为啥 main main main 要有，但 Entry Point 并不一定是要 main 方法，是可以自定义的。

![](Untitled-28acab3d-377c-4ca0-a69c-414914375be6.png)

![](Untitled-8b4a9267-f862-4b1d-a4da-bb07c2d2ce5f.png)

通过下面的例子，可以更好理解 Linker 是如何工作的。

# error LNK 2019: Unresolved External Symbol

### 复现

![](Untitled-02da08c2-5f77-4476-bde2-b593495a2dd1.png)

编译和 Bulid 都没有问题，然后

    void Log -> void Logr

会发现 Math.cpp 可以正常编译。它能正常编译的原因是它仍然有 Log 函数的声明，于是编译器会相信在哪里有这样一个函数叫 Log，这样，它的编译就可以顺利通过了。

![](Untitled-22ac4fc8-5e9d-499d-8a54-e387cd1455f9.png)

然而 Build 一下项目，Unresolved External Symbol 就复现了

![](Untitled-d106b942-4d8c-45fc-929a-0df74c60da07.png)

出现错误的原因是在 Linking 时，会去找这个叫 Log 的函数，然而 Linker 找不到，就报这个错了，错误信息中还告诉了我们具体丢失的 Symbol。

    error LNK2019: unresolved external symbol "void __cdecl Log(char const *)" (?Log@@YAXPBD@Z) referenced in function "int __cdecl Multiply(int,int)" (?Multiply@@YAHHH@Z)

字面意思，在方法 Multiply 中，我们调用的 Log 函数丢失，找不到该 Link 到哪去。

于是我们把调用 Log 的代码注释掉，就发现 Build 顺利通过。

![](Untitled-5633ba27-6ba2-467e-acc8-fd1f2e68328b.png)

这是因为编译器知道我们从来没调用过 Log 函数，那 Link 当然就不会出问题了。继续。

    Log("Multyply");
    ···
    //std::cout << Multiply(5, 8) << std::endl;

重新调用 Log，不再调用 Multiply，会发生什么？是的，相同的错误又来了。 

按道理没有调用 Multiply 之后，就死代码了，但还会报 Linking Error，所以我们要主动告诉编译器 这个函数只在这个编译单元里被使用，就不会报错了。

![](Untitled-b9bb3f7b-30a8-483f-9a4a-7e1d465431c3.png)

# error LNK1169/LNK2005

当我们有函数或变量有相同名字和相同签名时，也就是两个相同名称的函数具有相同的返回值和相同的参数，会报 Linking 相关的错。

### 复现

Log.cpp 中，写两组完全相同的函数，Build 项目。

    #include <iostream>
    
    void Log(const char* message)
    {
    	std::cout << message << std::endl;
    }
    
    void Log(const char* message)
    {
    	std::cout << message << std::endl;
    }

![](Untitled-4cc11fe1-fb26-4e90-aaaa-5f97c4fd2a89.png)

error CXXXX，代表编译错误，这种情况编译器会告诉你哪里错。

如果把 Log 函数移到 Math.cpp 中，编译错误就不会发生，但是 Build 项目后。

![](Untitled-7b548349-b2e5-4630-aa5d-b9d35a3eca92.png)

这是因为俩 Log 函数完全一致，Linker 不知道到底该 Link 到哪个 Log 函数去，报错。

还有几种复现这种错误的情况

如图，新建 Header，定义 Log 函数，再给俩编译单元 include 一下，Build，报相同的错。

![](Untitled-422dc112-44a0-43ba-82dd-7ad7ff071143.png)

这是因为 include 的工作原理导致等于说在俩编译单元里，又同时都有了 Log 函数。出现这种情况就有好几种解决方案。

### 三种解决方案

- 标记 Log 函数为 static， 这样 Linker 在 Linking 时，这个 Log 函数只会在 Log.h 中被链接

    static void Log(const char* message)
    {
    	std::cout << message << std::endl;
    }

也就是说，俩编译单元里的 Log函数 其实可以看作是两个各自版本的 Log 函数，是不同的，于是编译到的中间件 obj 中，互相不可见。顺利 Build。

- 标记 Log 函数为 inline

    inline void Log(const char* message)
    {
    	std::cout << message << std::endl;
    }

👇 inline 的作用是将 Log 函数的 Body 拿到被调用的编译单元中取代调用，顺利 Build

    void LogInit()
    {
    	Log("Log Init"); -> std::cout << "Log Init" << std::endl;
    }

- Log.h 中仅做 Log 的声明，Log函数 回归 Log.cpp，这样 Math.cpp 调用 Log 函数后，因为只定义了一次，就也能顺利 Build。这种方法最推荐。

![](Untitled-9607f375-3292-4441-9865-390627dbb251.png)