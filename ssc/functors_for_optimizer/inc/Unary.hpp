/*
 * Unary.hpp
 *
 * \date 1 f�vr. 2013, 12:33:33
 *  \author cec
 */

#ifndef UNARY_HPP_
#define UNARY_HPP_

#include "Node.hpp"


class Unary : public Node
{

    public:
        Unary(const NodePtr& n_);
        NodePtr get_son() const;
        void accept(NodeVisitor& v) const;
        virtual std::string get_operator_name() const = 0;
    protected:
        NodePtr n;
        void set_value(const std::function<double()>& val);
    private:
        Unary();
};


#endif /* UNARY_HPP_ */