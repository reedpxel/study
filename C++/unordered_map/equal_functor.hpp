template <typename T>
struct EqualFunctor
{
    bool operator()(const T& left, const T& right)
    {
        return left == right;
    }
};
