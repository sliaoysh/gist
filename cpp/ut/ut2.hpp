class c
{
public:
    c(int i): x_{i}{}
private:
    friend class testing;
    int x_;
};
