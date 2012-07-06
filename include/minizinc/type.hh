#ifndef __MINIZINC_TYPE_HH__
#define __MINIZINC_TYPE_HH__

#include <string>
#include <sstream>

namespace MiniZinc {

  class Type {
  public:
    enum TypeInst { TI_PAR, TI_VAR, TI_SVAR, TI_ANY };
    enum BaseType { BT_BOOL, BT_INT, BT_FLOAT, BT_STRING, BT_ANN,
                    BT_BOT, BT_UNKNOWN };
    enum SetType { ST_PLAIN, ST_SET };
    TypeInst _ti : 3;
    BaseType _bt : 3;
    SetType _st  : 1;
    unsigned int _dim : 20;
    Type(void) : _ti(TI_PAR), _bt(BT_UNKNOWN), _st(ST_PLAIN), _dim(0) {}
  protected:
    Type(const TypeInst& ti, const BaseType& bt, const SetType& st,
         unsigned int dim)
      : _ti(ti), _bt(bt), _st(st), _dim(dim) {
      assert(dim < 1<<20);
    }
  public:
    static Type any(unsigned int dim=0) {
      return Type(TI_ANY,BT_INT,ST_PLAIN,dim);
    }
    static Type parint(unsigned int dim=0) {
      return Type(TI_PAR,BT_INT,ST_PLAIN,dim);
    }
    static Type parbool(unsigned int dim=0) {
      return Type(TI_PAR,BT_BOOL,ST_PLAIN,dim);
    }
    static Type parfloat(unsigned int dim=0) {
      return Type(TI_PAR,BT_FLOAT,ST_PLAIN,dim);
    }
    static Type parstring(unsigned int dim=0) {
      return Type(TI_PAR,BT_STRING,ST_PLAIN,dim);
    }
    static Type ann(unsigned int dim=0) {
      return Type(TI_PAR,BT_ANN,ST_PLAIN,dim);
    }
    static Type parsetint(unsigned int dim=0) {
      return Type(TI_PAR,BT_INT,ST_SET,dim);
    }
    static Type parsetbool(unsigned int dim=0) {
      return Type(TI_PAR,BT_BOOL,ST_SET,dim);
    }
    static Type parsetfloat(unsigned int dim=0) {
      return Type(TI_PAR,BT_FLOAT,ST_SET,dim);
    }
    static Type parsetstring(unsigned int dim=0) {
      return Type(TI_PAR,BT_STRING,ST_SET,dim);
    }
    static Type varint(unsigned int dim=0) {
      return Type(TI_VAR,BT_INT,ST_PLAIN,dim);
    }
    static Type varbool(unsigned int dim=0) {
      return Type(TI_VAR,BT_BOOL,ST_PLAIN,dim);
    }
    static Type varfloat(unsigned int dim=0) {
      return Type(TI_VAR,BT_FLOAT,ST_PLAIN,dim);
    }
    static Type varsetint(unsigned int dim=0) {
      return Type(TI_VAR,BT_INT,ST_SET,dim);
    }
    static Type bot(unsigned int dim=0) {
      return Type(TI_PAR,BT_BOT,ST_PLAIN,dim);
    }

    bool isunknown(void) const { return _bt==BT_UNKNOWN; }
    bool isany(void) const { return _ti==TI_ANY; }
    bool isplain(void) const { return _dim==0 && _st==ST_PLAIN; }
    bool isint(void) const { return isplain() && _bt==BT_INT; }
    bool isfloat(void) const { return isplain() && _bt==BT_FLOAT; }
    bool isbool(void) const { return isplain() && _bt==BT_BOOL; }
    bool isstring(void) const { return isplain() && _bt==BT_STRING; }
    bool isvar(void) const { return _ti!=TI_PAR; }
    bool issvar(void) const { return _ti==TI_SVAR; }
    bool ispar(void) const { return _ti==TI_PAR; }
    bool isset(void) const { return _dim==0 && _st==ST_SET; }
    bool isintset(void) const { return isset() && _bt==BT_INT; }
    bool isann(void) const { return isplain() && _bt==BT_ANN; }
    bool isintarray(void) const {
      return _dim==1 && _st==ST_PLAIN && _bt==BT_INT;
    }
    bool isboolarray(void) const {
      return _dim==1 && _st==ST_PLAIN && _bt==BT_BOOL;
    }
    bool isintsetarray(void) const {
      return _dim==1 && _st==ST_SET && _bt==BT_INT;
    }

    bool operator== (const Type& t) const {
      return _ti==t._ti && _bt==t._bt && _st==t._st && _dim==t._dim;
    }
    bool operator!= (const Type& t) const {
      return !this->operator==(t);
    }

    bool isSubtypeOf(const Type& t) const {
      if ((*this)==t)
        return true;
      if (isany() && !t.ispar())
        return true;
      if ((_ti==TI_PAR || _ti==TI_SVAR) && _bt==t._bt &&
          _dim==t._dim && _st==t._st)
        return true;
      if (t._ti==TI_SVAR && _bt==t._bt && _dim==t._dim && _st==t._st)
        return true;
      if (t.isany() && (t._dim==0 || _dim==t._dim))
        return true;
      if ( (_ti==TI_PAR || _ti==t._ti) && _bt==BT_BOT && _dim==t._dim)
        return true;
      return false;
    }
  // protected:
    unsigned int toInt(void) const {
      return
        (static_cast<unsigned>(_ti)<<24)
      + (static_cast<unsigned>(_bt)<<21)
      + (static_cast<unsigned>(_st)<<20)
      + _dim;
    }
    std::string toString(void) const {
      std::ostringstream oss;
      if (_dim>0)
        oss<<"array["<<_dim<<"]";
      switch (_ti) {
        case TI_PAR: oss<<"par "; break;
        case TI_VAR: oss<<"var "; break;
        case TI_SVAR: oss<<"svar "; break;
        case TI_ANY: oss<<"any "; break;
      }
      if (_st==ST_SET) oss<<"set of ";
      switch (_bt) {
        case BT_INT: oss<<"int"; break;
        case BT_BOOL: oss<<"bool"; break;
        case BT_FLOAT: oss<<"float"; break;
        case BT_STRING: oss<<"string"; break;
        case BT_ANN: oss<<"ann"; break;
        case BT_BOT: oss<<"bot"; break;
        case BT_UNKNOWN: oss<<"??? "; break;
      }
      return oss.str();
    }
  public:
    int cmp(const Type& t) const {
      return toInt()<t.toInt() ? -1 : (toInt()>t.toInt() ? 1 : 0);
    }

  };
  
};

#endif