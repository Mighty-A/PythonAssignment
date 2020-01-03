# Tutorial about Submitting the Bighomework

Since many of us have done a lot in this Bighomework, this is a tutorial about submitting the Bighomework to OJ to check the scores you can get now.
***
## Step 0: Make sure the code can be compiled and pass most of tests given (maybe some except).
You can run in the terminal the following code to check all the tests given:
    
    python3 ./test.py

if the result is like this 

    ./python_interpreter testData/test0.in > temp/test0.out
    ./python_interpreter testData/test1.in > temp/test1.out
    ./python_interpreter testData/test2.in > temp/test2.out
    ./python_interpreter testData/test3.in > temp/test3.out
    ./python_interpreter testData/test4.in > temp/test4.out
    ./python_interpreter testData/test5.in > temp/test5.out
    ./python_interpreter testData/test6.in > temp/test6.out
    ./python_interpreter testData/test7.in > temp/test7.out
    ....

it means you've got the right answer of this tests. (this tests can be found in the folder)  

or maybe there is some wrong and you need to deeeeeeeeeeeeebug.   :(   
if you pass the most of them, you can try to submit it.
***
## Step 1: make a new folder and copy the <u>core code</u> into it
Make a new folder(NOT in the folder of homework)
such as  

    yhy@YHY-SURFACE:~/homeworkUpload  

Copy the following folders and files into it  

    generated
    src
    third_party
    CMakeLists.txt

Attention: makesure you copy all of the files, not a empty folder. Don't Cmake or make in the new folder(homeworkUpload).

## Step 2: change some code in following files
#### 1.main.cpp
CHANGE
```c++
    ANTLRInputStream input(ifs); 
    ↓
    ANTLRInputStream input(std::cin);
```

DELETE
```c++
    const std::string filepath=argv[1];
    std::ifstream ifs;
    ifs.open(filepath);
    if (!ifs.good()) {
        std::cout<<"bad";
    }
    ...
    ifs.close();
```

#### 2.CMakeLists.txt
CHANGE
```cmake
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}   -g")
    ↓
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}   -O3")


    add_executable(python_interpreter ${src_dir} src/main.cpp)
    target_link_libraries(python_interpreter antlr4-cpp-runtime)
    ↓
    add_executable(code ${src_dir} src/main.cpp)
    target_link_libraries(code antlr4-cpp-runtime)
```
***

## Step 3: Push the code to your github account

you may need some basic code like:

    git init
    git add .
    git commit -m "I MAKE IT!!!!"
    git push

To push successfully, you need to know something about git  
visit this website for some introduction :

[廖雪峰GIT教程](https://www.liaoxuefeng.com/wiki/896043488029600)

(or ask TA or yhy through WeChat and QQ)
***
## Step 4: Submit
open the homework in the oj:

http://106.54.133.230/problem/2

click upload and copy the HTTPS from your github repositories to the website like:

    https://github.com/Mighty-A/homeworkUpload.git

***
## Step 5: Waiting
It dose take a long time to run your code

***
## Step 6: Checking
if you meet problems like: Compile Error  
click the Git to check what is wrong

if the runtime is not 0, it means your code can be successfully compiled(but there may be System Error or Wrong Answer).Then you can deeeeeeeeebug. :)

### Hope this Tutorial can help

