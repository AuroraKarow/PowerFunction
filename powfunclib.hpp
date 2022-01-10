POWFUNC_BEGIN

double pi_lib(double acc = 1e-5)
{
    auto ans = 3.0, curr_ans = 3.0;
    do
    {
        ans = curr_ans;
        curr_ans = ans - std::sin(ans) / std::cos(ans);
    }
    while (std::abs(curr_ans-ans) > acc);
    return acc_round(ans, acc);
}

double euler_itr_lib(double times, uint64_t k = 0) { return (1 + 2 * k) * pi_lib() * times;}

net_list<std::complex<double>> euler_eq_lib(double times, double acc = 1e-5)
{
    net_list<std::complex<double>> ans;
    net_list<bool> ans_flag;
    auto cnt = 0;
    bool flag = true;
    while(flag)
    {
        if(cnt > 8) break;
        auto curr_itr = euler_itr_lib(times, cnt);
        auto real = acc_round(std::cos(curr_itr), acc), img = acc_round(std::sin(curr_itr), acc);
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
    }
    return ans;
}

double power_lib(double base, double times, double acc = 1e-5)
{
    if(base)
        if(base > 0) return exp(times * log(base));
        else
        {
            auto euler_res = euler_eq_lib(times, acc);
            bool flag = true;
            auto euler_ans = 0;
            if(euler_res.size()) for(auto i=0; i<euler_res.size(); ++i)
            {
                auto curr_cplx = euler_res[i];
                if(curr_cplx.imag() == 0) return exp(times * log((-1) * base)) * curr_cplx.real();
            }
            return NAN;
        }
    else return 0;
}

POWFUNC_END