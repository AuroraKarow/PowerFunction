POWFUNC_BEGIN

template<typename _T> void quick_sort(std::unique_ptr<_T[]> &seq_val, uint64_t begin, uint64_t end, std::function<bool(_T&, _T&)>func_comp = [](_T &_first, _T &_second){return _first > _second;})
{
    if(end == begin) return;
    else
    {
        auto pivot = begin, slide = end;
        while(slide != pivot)
            if(pivot<slide)
            {
                if(!func_comp(seq_val[slide], seq_val[pivot]))
                {
                    std::swap(seq_val[slide], seq_val[pivot]);
                    std::swap(slide, pivot);
                    slide ++;
                }
                else -- slide;
            }
            else
            {
                if(func_comp(seq_val[slide], seq_val[pivot]))
                {
                    std::swap(seq_val[slide], seq_val[pivot]);
                    std::swap(slide, pivot);
                    -- slide;
                }
                else ++ slide;
            }
        if(pivot != begin) quick_sort(seq_val, begin, pivot-1, func_comp);
        if(pivot != end) quick_sort(seq_val, pivot+1, end, func_comp);
    }
}

template<typename T> class net_queue
{
protected:
    std::unique_ptr<T[]> ptr;
    uint64_t len = 0;
    void _alloc(uint64_t _size)
    {
        ptr = std::make_unique<T[]>(_size);
        len = _size;
    }
    T temp;
public:
    net_queue(uint64_t _size = 1) { _alloc(_size); }
    net_queue(net_queue &src)
    {
        if(len != src.len) _alloc(src.len);
        for(auto i=0; i<len; ++i) ptr[i] = src.ptr[i];
    }
    net_queue(net_queue &&src) : len(src.len)
    {
        ptr = std::move(src.ptr);
        src.len = 0;
    }
    uint64_t size() { return len; }
    template<typename ... args> bool insert(uint64_t idx, args &&...src)
    {
        if(idx > len) return false;
        else
        {
            auto _ptr = std::make_unique<T[]>(len + 1);
            ++ len;
            for(auto i=0; i<idx; ++i) _ptr[i] = std::move(ptr[i]);
            _ptr[idx] = std::move(T(std::forward<args>(src)...));
            for(auto i=idx+1; i<len; ++i) _ptr[i] = std::move(ptr[i-1]);
            ptr = std::move(_ptr);
            return true;
        }
    }
    T &erase(uint64_t idx)
    {
        if(idx < len)
        {
            auto _ptr = std::make_unique<T[]>(len - 1);
            -- len;
            for(auto i=0; i<idx; ++i) _ptr[i] = std::move(ptr[i]);
            temp = std::move(ptr[idx]);
            for(auto i=idx; i<len; ++i) _ptr[i] = std::move(ptr[i+1]);
            ptr = std::move(_ptr);
        }
        return temp;
    }
    void sort(std::function<bool(T&, T&)> _func = [](T &_first, T &_second){return _first > _second;}) {quick_sort(ptr, 0, len - 1, _func);}
    void operator=(net_queue &src) { new (this)net_queue(src); }
    void operator=(net_queue &&src) { new (this)net_queue(std::move(src)); }
    bool operator==(net_queue &src)
    {
        if(len == src.len)
        {
            for(auto i=0; i<len; ++i) if(ptr[i] != src.ptr[i]) return false;
            return true;
        }
        else return false;
    }
    bool operator!=(net_queue &src) { return !(*this == src); }
    T &operator[](uint64_t idx) { return ptr[idx]; }
    friend std::ostream &operator<<(std::ostream &output, net_queue &src)
    {
        for(auto i=0; i<src.len; ++i)
        {
            output << '[' << i << "][" << std::endl << src.ptr[i] << std::endl << ']';
            if(i+1 != src.len) output << std::endl;
        }
        return output;
    }
    ~net_queue(){}
};

template<typename T> class net_list
{
protected:
    struct node {node *prev = nullptr; std::unique_ptr<node> next = nullptr /* node *next */; T elem;};
    std::unique_ptr<node> head;
    node *tail = nullptr, *itr = nullptr;
    uint64_t len = 0, itr_idx = 0;
    T temp;
    std::unique_ptr<node> create_node() { return std::make_unique<node>(); }
    node *idx_node(uint64_t idx)
    {
        if(idx)
            if(idx+1 == len) itr = tail;
            else
            {
                auto tml_cnt = len - 1 - idx,
                    fnt_cnt = idx;
                uint64_t itr_mov = std::abs((int)itr_idx - (int)idx);
                if(itr && itr_mov<tml_cnt && itr_mov<fnt_cnt) while(itr_mov)
                {
                    if(itr_idx < idx) itr = (itr->next).get();
                    else itr = itr -> prev;
                    -- itr_mov;
                }
                else if(fnt_cnt < itr_mov)
                {
                    itr = head.get();
                    while(fnt_cnt)
                    {
                        itr = (itr->next).get();
                        -- fnt_cnt;
                    }
                }
                else
                {
                    itr = tail;
                    while(tml_cnt)
                    {
                        itr = itr -> prev;
                        -- tml_cnt;
                    }
                }
            }
        else itr = head.get();
        itr_idx = idx;
        return itr;
    }
public:
    uint64_t size() { return len; }
    net_list() {}
    net_list(net_list &src) : len(src.len)
    {
        if(len)
        {
            head = create_node();
            auto src_ptr = src.head.get(),
                temp_ptr = head.get();
            temp_ptr -> elem = src_ptr -> elem;
            tail = temp_ptr;
            for(auto i=1; i<len; ++i)
            {
                (temp_ptr -> next) = create_node();
                (temp_ptr -> next).get() -> prev = temp_ptr;
                (temp_ptr -> next).get() -> elem = src_ptr -> next -> elem;
                temp_ptr = (temp_ptr -> next).get();
                tail = temp_ptr;
                src_ptr = (src_ptr -> next).get();
            }
        }
    }
    net_list(net_list &&src) : head(std::move(src.head)), len(src.len), tail(src.tail)
    {
        src.tail = nullptr;
        src.len = 0;
    }
    template<typename...args>bool insert(uint64_t idx, args &&...src)
    {
        if(idx > len) return false;
        else 
        {
            auto temp_ptr = create_node();
            auto temp_node = temp_ptr.get();
            temp_node -> elem = std::move(T(std::forward<args>(src)...));
            if(idx)
                if(idx == len)
                {
                    if(tail)
                    {
                        tail -> next = std::move(temp_ptr);
                        temp_node -> prev = tail; 
                        tail = temp_node;
                    }
                    else
                    {
                        head = std::move(temp_ptr);
                        tail = head.get();
                    }
                    itr = tail;
                }
                else
                {
                    auto tgt_node = idx_node(idx);
                    temp_node -> prev = tgt_node -> prev;
                    tgt_node -> prev = temp_node;
                    temp_node -> next = std::move(temp_node -> prev -> next);
                    temp_node -> prev -> next = std::move(temp_ptr);
                    itr = temp_node;
                }
            else
            {
                temp_node -> next = std::move(head);
                head = std::move(temp_ptr);
                if(temp_node -> next) (temp_node->next).get() -> prev = temp_node;
                else tail = temp_node;
                itr = head.get();
            }
            ++ len;
            itr_idx = idx;
            return true;
        }
    }
    T &erase(uint64_t idx)
    {
        if(idx < len)
        {
            auto tgt_node = idx_node(idx);
            temp = std::move(tgt_node -> elem);
            if(tgt_node -> prev)
            {
                tgt_node = tgt_node -> prev;
                tgt_node -> next = std::move((tgt_node->next).get()->next);
                if(tgt_node->next) (tgt_node->next).get()->prev = tgt_node;
                else tail = tgt_node;
                -- itr_idx;
                
            }
            else if(tgt_node -> next)
            {
                head = std::move(head.get()->next);
                tgt_node = head.get();
                tgt_node -> prev = nullptr;
            }
            else
            {
                tgt_node = nullptr;
                head.reset();
            }
            itr = tgt_node;
            -- len;
        }
        return temp;
    }
    net_list unit(net_list &src)
    {
        net_list tool_cpy = *this;
        if(src.len)
        {
            net_list src_cpy = src;
            tool_cpy.len += src_cpy.len;
            src_cpy.head.get() -> prev = tool_cpy.tail;
            tool_cpy.tail -> next = std::move(src_cpy.head);
            tool_cpy.tail = src_cpy.tail;
        }
        return tool_cpy;
    }
    net_list colabo_unit(net_list &src)
    {
        net_list src_cpy = src, tool_cpy = *this;
        for(auto i=0; i<len; ++i) for(auto j=0; j<src_cpy.len; ++j) if(src_cpy[j] == tool_cpy[i])
        {
            src_cpy.erase(j);
            break;
        }
        return tool_cpy.unit(src_cpy);
    }
    net_list inters_unit(net_list &src)
    {
        net_list src_cpy = src, ls_temp;
        for(auto i=0; i<len; ++i) for(auto j=0; j<src_cpy.len; ++j) if(src_cpy[j] == (*this)[i])
        {
            ls_temp.insert(ls_temp.len, src_cpy.erase(j));
            break;
        }
        return ls_temp;
    }
    T &operator[](uint64_t idx)
    {
        if(idx < len) return idx_node(idx) -> elem;
        else return temp;
    }
    void operator=(net_list &src) {new (this)net_list(src);}
    void operator=(net_list &&src) {new (this)net_list(std::move(src));}
    bool operator==(net_list &src)
    {
        if(len == src.len)
        {
            auto tool_ptr = head.get(),
                src_ptr = src.head.get();
            for(auto i=0; i<len; ++i)
                if(tool_ptr->elem != src_ptr->elem) return false;
                else
                {
                    tool_ptr = (tool_ptr -> next).get();
                    src_ptr = (src_ptr -> next).get();
                }
            return true;
        }
        else return false;
    }
    bool operator!=(net_list &src) {return !(*this == src);}
    friend std::ostream &operator<<(std::ostream &output, net_list &src)
    {
        auto tool_ptr = src.head.get();
        for(auto i=0; i<src.len; ++i)
        {
            output << '[' << i << "][" << std::endl << tool_ptr -> elem << std::endl << ']';
            if(i+1 != src.len) output << std::endl;
            tool_ptr = (tool_ptr -> next).get();
        }
        return output;
    }
    // ~net_list() {}
};

net_list<uint64_t> prime(uint64_t upper)
{
    net_list<uint64_t> ls_prime;
    ls_prime.insert(0, 2);
    for(auto i=3; i<upper; ++i)
    {
        auto flag = true;
        for(auto j=0; j<ls_prime.size(); ++j) if(i%ls_prime[j] == 0)
        {
            flag = false;
            break;
        }
        if(flag) ls_prime.insert(ls_prime.size(), i);
    }
    return ls_prime;
}

double acc_norm(double acc)
{
    if(acc<1 && acc>0)
    {
        auto _acc = acc;
        auto bit_cnt = 1;
        while(_acc > 0)
        {
            bit_cnt *= 10;
            _acc *= 10;
            _acc -= (int)_acc;
        }
        acc = 1.0 / bit_cnt;
    }
    return acc;
}

double acc_round(double _val, double acc = 1e-5)
{
    acc = acc_norm(acc);
    auto _itgr = (int)_val;
    auto _dcml = (_val - _itgr) / acc;
    _dcml -= (int)_dcml;
    _dcml *= acc;
    _val -= _dcml;
    if(_dcml < 0.5*acc) return _val;
    else return _val + acc;
}

double sin(double rad, double acc = 1e-6)
{
    auto ans = rad, pow_num = rad, curr_ans = rad;
    long long frct_num = 1, sgn = -1, cnt = 1;
    do
    {
        ans = curr_ans;
        pow_num *= rad * rad;
        frct_num *= (cnt + 1) * (cnt + 2);
        curr_ans = ans + sgn * pow_num / frct_num;
        sgn *= (-1);
        cnt += 2;
    }
    while (std::abs(curr_ans-ans) > acc);
    return acc_round(ans, acc);
}

double cos(double rad, double acc = 1e-6)
{
    auto ans = 1.0, pow_num = rad * rad, curr_ans = 1.0;
    long long frct_num = 1, sgn = -1, cnt = 0;
    do
    {
        ans = curr_ans;
        frct_num *= (cnt + 1) * (cnt + 2);
        curr_ans = ans + sgn * pow_num / frct_num;
        pow_num *= rad * rad;
        cnt += 2;
        sgn *= (-1);
    }
    while (std::abs(curr_ans-ans) > acc);
    return acc_round(ans, acc);
}

double pi(double acc = 1e-6)
{
    auto ans = 3.0, curr_ans = 3.0;
    do
    {
        ans = curr_ans;
        curr_ans = ans - sin(ans) / cos(ans);
    }
    while (std::abs(curr_ans-ans) > acc);
    return acc_round(ans, acc);
}

double ln(double natural, double acc = 1e-6)
{
    if(natural > 0)
    {
        natural = (natural - 1) / (natural + 1);
        auto ans = natural, curr_ans = natural, pow_num = natural;
        auto cnt = 1;
        do
        {
            ans = curr_ans;
            cnt += 2;
            pow_num *= natural * natural;
            curr_ans = ans + pow_num / cnt;
        } while (std::abs(curr_ans-ans) > acc);
        return 2 * acc_round(ans, acc);
    }
    else return NAN;
}

double euler_itr(double times, uint64_t k = 0) { return (1 + 2 * k) * pi() * times;}

net_list<std::complex<double>> euler_eq(double times, double acc = 1e-3)
{
    net_list<std::complex<double>> ans;
    net_list<bool> ans_flag;
    auto cnt = 0;
    bool flag = true;
    while(flag)
    {
        auto curr_itr = euler_itr(times, cnt);
        auto real = cos(curr_itr, acc),
            img = sin(curr_itr, acc);
        if(isnan(real) || isnan(img)) break;
        else 
        {
            std::complex<double> curr_ans(real, img);
            for(auto i=0; i<ans.size(); ++i)
            {
                if(ans[i] == curr_ans)
                {
                    ans_flag[i] = true;
                    flag = false;
                    break;
                }
            }
            if(flag)
            {
                ans.insert(ans.size(), curr_ans);
                ans_flag.insert(ans_flag.size(), false);
            }
            else for(auto i=0; i<ans_flag.size(); ++i) if(!ans_flag[i])
            {
                flag = true;
                break;
            }
            ++ cnt;
        }
        // std::cout <<"[round] " << cnt << std::endl << ans << std::endl << std::endl;
    }
    return ans;
}

double exponent(double times, double acc = 1e-6)
{
    auto ans = 1.0, pow_num = times, curr_ans = 1.0;
    long long fact_num = 1, cnt = 1;
    do
    {
        ans = curr_ans;
        curr_ans = ans + pow_num / (fact_num);
        ++ cnt;
        fact_num *= cnt;
        pow_num *= times;
    }
    while (std::abs(curr_ans-ans) > acc);
    return acc_round(ans, acc);
}

double power(double base, double times, double acc = 1e-10)
{
    if(base)
        if(base > 0) return exponent(times * ln(base, acc), acc);
        else
        {
            auto euler_res = euler_eq(times);
            bool flag = true;
            auto euler_ans = 0;
            if(euler_res.size()) for(auto i=0; i<euler_res.size(); ++i)
            {
                auto curr_cplx = euler_res[i];
                if(curr_cplx.imag() == 0) return exponent(times * ln((-1) * base, acc), acc) * curr_cplx.real();
            }
            return NAN;
        }
    else return 0;
}

POWFUNC_END