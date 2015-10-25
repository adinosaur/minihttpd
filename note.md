develop note
===========

#### Date 2015/10/24 bug
```c++
template <typename T>
class TrieTreeNote
{
    public:
        void set_val(const T& val)
        {
            destroy();
            val = new T(v);
        }   

    private:
        T* val = nullptr;
};
```
对于上面代码报错
```
./test/../include/TrieTree.h:45:17: error: no viable overloaded '='
    val = new T(val);
```
问题是由于命名冲突引起，类私有变量val和函数参数同名，导致变量查找被屏蔽。
