/*
 * Copyright (c) 2015 Andrew Kelley
 *
 * This file is part of zig, which is MIT licensed.
 * See http://opensource.org/licenses/MIT
 */

#ifndef ZIG_PARSER_HPP
#define ZIG_PARSER_HPP

#include "list.hpp"
#include "buffer.hpp"
#include "tokenizer.hpp"
#include "errmsg.hpp"

struct AstNode;
struct CodeGenNode;
struct ImportTableEntry;
struct AsmToken;

enum NodeType {
    NodeTypeRoot,
    NodeTypeRootExportDecl,
    NodeTypeFnProto,
    NodeTypeFnDef,
    NodeTypeFnDecl,
    NodeTypeParamDecl,
    NodeTypeType,
    NodeTypeBlock,
    NodeTypeExternBlock,
    NodeTypeDirective,
    NodeTypeReturnExpr,
    NodeTypeVariableDeclaration,
    NodeTypeBinOpExpr,
    NodeTypeCastExpr,
    NodeTypeNumberLiteral,
    NodeTypeStringLiteral,
    NodeTypeCharLiteral,
    NodeTypeUnreachable,
    NodeTypeSymbol,
    NodeTypePrefixOpExpr,
    NodeTypeFnCallExpr,
    NodeTypeArrayAccessExpr,
    NodeTypeSliceExpr,
    NodeTypeFieldAccessExpr,
    NodeTypeUse,
    NodeTypeVoid,
    NodeTypeBoolLiteral,
    NodeTypeNullLiteral,
    NodeTypeIfBoolExpr,
    NodeTypeIfVarExpr,
    NodeTypeWhileExpr,
    NodeTypeLabel,
    NodeTypeGoto,
    NodeTypeBreak,
    NodeTypeContinue,
    NodeTypeAsmExpr,
    NodeTypeStructDecl,
    NodeTypeStructField,
    NodeTypeStructValueExpr,
    NodeTypeStructValueField,
    NodeTypeCompilerFnExpr,
    NodeTypeCompilerFnType,
};

struct AstNodeRoot {
    ZigList<AstNode *> top_level_decls;
};

enum VisibMod {
    VisibModPrivate,
    VisibModPub,
    VisibModExport,
};

struct AstNodeFnProto {
    ZigList<AstNode *> *directives;
    VisibMod visib_mod;
    Buf name;
    ZigList<AstNode *> params;
    AstNode *return_type;
    bool is_var_args;
};

struct AstNodeFnDef {
    AstNode *fn_proto;
    AstNode *body;
};

struct AstNodeFnDecl {
    AstNode *fn_proto;
};

struct AstNodeParamDecl {
    Buf name;
    AstNode *type;
};

enum AstNodeTypeType {
    AstNodeTypeTypePrimitive,
    AstNodeTypeTypePointer,
    AstNodeTypeTypeArray,
    AstNodeTypeTypeMaybe,
    AstNodeTypeTypeCompilerExpr,
};

struct AstNodeType {
    AstNodeTypeType type;
    Buf primitive_name;
    AstNode *child_type;
    AstNode *array_size; // can be null
    bool is_const;
    bool is_noalias;
    AstNode *compiler_expr;
};

struct AstNodeBlock {
    ZigList<AstNode *> statements;
};

struct AstNodeReturnExpr {
    // might be null in case of return void;
    AstNode *expr;
};

struct AstNodeVariableDeclaration {
    Buf symbol;
    bool is_const;
    VisibMod visib_mod;
    // one or both of type and expr will be non null
    AstNode *type;
    AstNode *expr;
};

enum BinOpType {
    BinOpTypeInvalid,
    BinOpTypeAssign,
    BinOpTypeAssignTimes,
    BinOpTypeAssignDiv,
    BinOpTypeAssignMod,
    BinOpTypeAssignPlus,
    BinOpTypeAssignMinus,
    BinOpTypeAssignBitShiftLeft,
    BinOpTypeAssignBitShiftRight,
    BinOpTypeAssignBitAnd,
    BinOpTypeAssignBitXor,
    BinOpTypeAssignBitOr,
    BinOpTypeAssignBoolAnd,
    BinOpTypeAssignBoolOr,
    BinOpTypeBoolOr,
    BinOpTypeBoolAnd,
    BinOpTypeCmpEq,
    BinOpTypeCmpNotEq,
    BinOpTypeCmpLessThan,
    BinOpTypeCmpGreaterThan,
    BinOpTypeCmpLessOrEq,
    BinOpTypeCmpGreaterOrEq,
    BinOpTypeBinOr,
    BinOpTypeBinXor,
    BinOpTypeBinAnd,
    BinOpTypeBitShiftLeft,
    BinOpTypeBitShiftRight,
    BinOpTypeAdd,
    BinOpTypeSub,
    BinOpTypeMult,
    BinOpTypeDiv,
    BinOpTypeMod,
    BinOpTypeUnwrapMaybe,
};

struct AstNodeBinOpExpr {
    AstNode *op1;
    BinOpType bin_op;
    AstNode *op2;
};

struct AstNodeFnCallExpr {
    AstNode *fn_ref_expr;
    ZigList<AstNode *> params;
    bool is_builtin;
};

struct AstNodeArrayAccessExpr {
    AstNode *array_ref_expr;
    AstNode *subscript;
};

struct AstNodeSliceExpr {
    AstNode *array_ref_expr;
    AstNode *start;
    AstNode *end;
    bool is_const;
};

struct AstNodeFieldAccessExpr {
    AstNode *struct_expr;
    Buf field_name;
};

struct AstNodeExternBlock {
    ZigList<AstNode *> *directives;
    ZigList<AstNode *> fn_decls;
};

struct AstNodeDirective {
    Buf name;
    Buf param;
};

struct AstNodeRootExportDecl {
    Buf type;
    Buf name;
    ZigList<AstNode *> *directives;
};

struct AstNodeCastExpr {
    AstNode *expr;
    AstNode *type;
};

enum PrefixOp {
    PrefixOpInvalid,
    PrefixOpBoolNot,
    PrefixOpBinNot,
    PrefixOpNegation,
    PrefixOpAddressOf,
    PrefixOpConstAddressOf,
    PrefixOpDereference,
};

struct AstNodePrefixOpExpr {
    PrefixOp prefix_op;
    AstNode *primary_expr;
};

struct AstNodeUse {
    Buf path;
    ZigList<AstNode *> *directives;
};

struct AstNodeIfBoolExpr {
    AstNode *condition;
    AstNode *then_block;
    AstNode *else_node; // null, block node, or other if expr node
};

struct AstNodeIfVarExpr {
    AstNodeVariableDeclaration var_decl;
    AstNode *then_block;
    AstNode *else_node; // null, block node, or other if expr node
};

struct AstNodeWhileExpr {
    AstNode *condition;
    AstNode *body;
};

struct AstNodeLabel {
    Buf name;
};

struct AstNodeGoto {
    Buf name;
};

struct AsmOutput {
    Buf asm_symbolic_name;
    Buf constraint;
    Buf variable_name;
    AstNode *return_type; // null unless "=r" and return
};

struct AsmInput {
    Buf asm_symbolic_name;
    Buf constraint;
    AstNode *expr;
};

struct SrcPos {
    int line;
    int column;
};

struct AstNodeAsmExpr {
    bool is_volatile;
    Buf asm_template;
    ZigList<SrcPos> offset_map;
    ZigList<AsmToken> token_list;
    ZigList<AsmOutput*> output_list;
    ZigList<AsmInput*> input_list;
    ZigList<Buf*> clobber_list;
    int return_count; // populated by analyze
};

struct AstNodeStructDecl {
    Buf name;
    ZigList<AstNode *> fields;
    ZigList<AstNode *> fns;
    ZigList<AstNode *> *directives;
    VisibMod visib_mod;
};

struct AstNodeStructField {
    Buf name;
    AstNode *type;
    ZigList<AstNode *> *directives;
};

struct AstNodeStringLiteral {
    Buf buf;
    bool c;
};

struct AstNodeCharLiteral {
    uint8_t value;
};

enum NumLit {
    NumLitF32,
    NumLitF64,
    NumLitF128,
    NumLitU8,
    NumLitU16,
    NumLitU32,
    NumLitU64,
    NumLitI8,
    NumLitI16,
    NumLitI32,
    NumLitI64,

    NumLitCount
};

struct AstNodeNumberLiteral {
    NumLit kind;

    // overflow is true if when parsing the number, we discovered it would not
    // fit without losing data in a uint64_t, int64_t, or double
    bool overflow;

    union {
        uint64_t x_uint;
        int64_t x_int;
        double x_float;
    } data;
};

struct AstNodeStructValueField {
    Buf name;
    AstNode *expr;
};

struct AstNodeStructValueExpr {
    AstNode *type;
    ZigList<AstNode *> fields;
};

struct AstNodeCompilerFnExpr {
    Buf name;
    AstNode *expr;
};

struct AstNodeCompilerFnType {
    Buf name;
    AstNode *type;
};

struct AstNode {
    enum NodeType type;
    int line;
    int column;
    CodeGenNode *codegen_node;
    ImportTableEntry *owner;
    union {
        AstNodeRoot root;
        AstNodeRootExportDecl root_export_decl;
        AstNodeFnDef fn_def;
        AstNodeFnDecl fn_decl;
        AstNodeFnProto fn_proto;
        AstNodeType type;
        AstNodeParamDecl param_decl;
        AstNodeBlock block;
        AstNodeReturnExpr return_expr;
        AstNodeVariableDeclaration variable_declaration;
        AstNodeBinOpExpr bin_op_expr;
        AstNodeExternBlock extern_block;
        AstNodeDirective directive;
        AstNodeCastExpr cast_expr;
        AstNodePrefixOpExpr prefix_op_expr;
        AstNodeFnCallExpr fn_call_expr;
        AstNodeArrayAccessExpr array_access_expr;
        AstNodeSliceExpr slice_expr;
        AstNodeUse use;
        AstNodeIfBoolExpr if_bool_expr;
        AstNodeIfVarExpr if_var_expr;
        AstNodeWhileExpr while_expr;
        AstNodeLabel label;
        AstNodeGoto go_to;
        AstNodeAsmExpr asm_expr;
        AstNodeFieldAccessExpr field_access_expr;
        AstNodeStructDecl struct_decl;
        AstNodeStructField struct_field;
        AstNodeStringLiteral string_literal;
        AstNodeCharLiteral char_literal;
        AstNodeNumberLiteral number_literal;
        AstNodeStructValueExpr struct_val_expr;
        AstNodeStructValueField struct_val_field;
        AstNodeCompilerFnExpr compiler_fn_expr;
        AstNodeCompilerFnType compiler_fn_type;
        Buf symbol;
        bool bool_literal;
    } data;
};

enum AsmTokenId {
    AsmTokenIdTemplate,
    AsmTokenIdPercent,
    AsmTokenIdVar,
};

struct AsmToken {
    enum AsmTokenId id;
    int start;
    int end;
};

__attribute__ ((format (printf, 2, 3)))
void ast_token_error(Token *token, const char *format, ...);


// This function is provided by generated code, generated by parsergen.cpp
AstNode * ast_parse(Buf *buf, ZigList<Token> *tokens, ImportTableEntry *owner, ErrColor err_color);

const char *node_type_str(NodeType node_type);

void ast_print(AstNode *node, int indent);

const char *num_lit_str(NumLit num_lit);
bool is_num_lit_unsigned(NumLit num_lit);
bool is_num_lit_float(NumLit num_lit);
uint64_t num_lit_bit_count(NumLit num_lit);


#endif
