# 涉及知识
#### 1.C语言——不完全类型
[百度百科](https://baike.baidu.com/item/%E4%B8%8D%E5%AE%8C%E5%85%A8%E7%B1%BB%E5%9E%8B/22749504?fr=aladdin)
不完整类型是尚未指定其成员的结构或联合类型，尚未指定其维度的数组类型，或void类型（无法完成void类型）。 这种类型可能无法实例化（其大小未知），也不能访问其成员（它们也是未知的）; 但是，可以使用派生的指针类型（但不能解除引用）。

> YBX.h中的 YBX_Interpreter 类型

#### 2.lacc
<INITIAL> 开始条件，在 ybx.l 中<INITIAL>分割“注释”和“字面值常量(literal)”