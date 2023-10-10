// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2022 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include "mpt_hash.hpp"
#include "account.hpp"
#include "mpt.hpp"
#include "rlp.hpp"
#include "state.hpp"

namespace evmone::state
{
namespace
{
hash256 mpt_hash(const std::unordered_map<hash256, StorageValue>& storage)
{
    MPT trie;
    for (const auto& [key, value] : storage)
    {
        if (!is_zero(value.current))  // Skip "deleted" values.
            trie.insert(keccak256(key), rlp::encode(rlp::trim(value.current)));
    }
    return trie.hash();
}
}  // namespace

hash256 mpt_hash(const std::unordered_map<address, Account>& accounts)
{
    MPT trie;
    for (const auto& [addr, acc] : accounts)
    {
        trie.insert(keccak256(addr),
            rlp::encode_tuple(acc.nonce, acc.balance, mpt_hash(acc.storage), keccak256(acc.code)));
    }
    return trie.hash();
}

template <typename T>
hash256 mpt_hash(std::span<const T> list)
{
    MPT trie;
    for (size_t i = 0; i < list.size(); ++i)
        trie.insert(rlp::encode(i), rlp::encode(list[i]));

    return trie.hash();
}

template hash256 mpt_hash<Transaction>(std::span<const Transaction>);
template hash256 mpt_hash<TransactionReceipt>(std::span<const TransactionReceipt>);
template hash256 mpt_hash<Withdrawal>(std::span<const Withdrawal>);

}  // namespace evmone::state
