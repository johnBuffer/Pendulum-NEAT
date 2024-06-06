#pragma once
#include <vector>


namespace siv
{

    using ID = uint64_t;

    /// Forward declaration
    template<typename TObjectType>
    class IndexVector;

    /** Standalone object to access an object
     *
     * @tparam TObjectType The object's type
     */
    template<typename TObjectType>
    class Ref
    {
    public:
        Ref() = default;
        /// Constructor
        Ref(ID id, ID validity_id, IndexVector<TObjectType>* vector)
                : m_id{id}
                , m_validity_id{validity_id}
                , m_vector{vector}
        {}

        TObjectType* operator->()
        {
            return &(*m_vector)[m_id];
        }

        const TObjectType* operator->() const
        {
            return &(*m_vector)[m_id];
        }

        TObjectType& operator*()
        {
            return (*m_vector)[m_id];
        }

        const TObjectType& operator*() const
        {
            return (*m_vector)[m_id];
        }

        [[nodiscard]]
        ID getID() const
        {
            return m_id;
        }

        /** Bool operator to test against the validity of the reference
         *
         * @return false if uninitialized or if the object has been erased from the vector, true otherwise
         */
        explicit operator bool() const
        {
            return m_vector && m_vector->isValid(m_id, m_validity_id);
        }

    private:
        ID                        m_id          = 0;
        ID                        m_validity_id = 0;
        IndexVector<TObjectType>* m_vector      = nullptr;
    };

    template<typename TObjectType>
    class IndexVector
    {
    public:
        IndexVector() = default;

        ID push_back(const TObjectType& object)
        {
            const ID id = getSlot();
            m_data.push_back(object);
            return id;
        }

        template<typename... TArgs>
        ID emplace_back(TArgs&&... args)
        {
            const ID id = getSlot();
            m_data.emplace_back(std::forward<TArgs>(args)...);
            return id;
        }

        void erase(ID id)
        {
            // Fetch relevant info
            const ID data_id      = m_indexes[id];
            const ID last_data_id = m_data.size() - 1;
            const ID last_id      = m_metadata[last_data_id].rid;
            // Invalidate m_metadata
            m_metadata[data_id].validity_id = operation_count++;
            // Swap the object to delete with the object at the end
            std::swap(m_data[data_id], m_data[last_data_id]);
            std::swap(m_metadata[data_id], m_metadata[last_data_id]);
            std::swap(m_indexes[id], m_indexes[last_id]);
            // Destroy the object
            m_data.pop_back();
        }

        void erase(const Ref<TObjectType>& ref)
        {
            erase(ref.getID());
        }

        ID getSlot()
        {
            const ID id = getSlotID();
            m_indexes[id] = m_data.size();
            return id;
        }

        ID getSlotID()
        {
            // This means that we have available slots
            if (m_metadata.size() > m_data.size()) {
                m_metadata[m_data.size()].validity_id = operation_count++;
                return m_metadata[m_data.size()].rid;
            }
            // A new slot has to be created
            const ID new_id = m_data.size();
            m_metadata.push_back({new_id, operation_count++});
            m_indexes.push_back(new_id);
            return new_id;
        }

        [[nodiscard]]
        uint64_t getDataIndex(ID id) const
        {
            return m_indexes[id];
        }

        TObjectType& operator[](ID id)
        {
            return m_data[m_indexes[id]];
        }

        const TObjectType& operator[](ID id) const
        {
            return m_data[m_indexes[id]];
        }

        [[nodiscard]]
        size_t size() const
        {
            return m_data.size();
        }

        [[nodiscard]]
        bool empty() const
        {
            return m_data.empty();
        }

        [[nodiscard]]
        size_t capacity() const
        {
            return m_data.capacity();
        }

        Ref<TObjectType> createRef(ID id)
        {
            return {id, m_metadata[m_indexes[id]].validity_id, this};
        }

        Ref<TObjectType> createRefFromData(uint64_t idx)
        {
            return {m_metadata[idx].rid, m_metadata[idx].validity_id, this};
        }

        [[nodiscard]]
        bool isValid(ID id, ID validity_id) const
        {
            return validity_id == m_metadata[m_indexes[id]].validity_id;
        }

        typename std::vector<TObjectType>::iterator begin() noexcept
        {
            return m_data.begin();
        }

        typename std::vector<TObjectType>::iterator end() noexcept
        {
            return m_data.end();
        }

        typename std::vector<TObjectType>::const_iterator begin() const noexcept
        {
            return m_data.begin();
        }

        typename std::vector<TObjectType>::const_iterator end() const noexcept
        {
            return m_data.end();
        }

        template<typename TCallback>
        void remove_if(TCallback&& callback)
        {
            for (uint32_t i{0}; i < m_data.size();) {
                if (callback(m_data[i])) {
                    erase(m_metadata[i].rid);
                } else {
                    ++i;
                }
            }
        }

        void reserve(size_t size)
        {
            m_data.reserve(size);
        }

        [[nodiscard]]
        ID getValidityID(ID id) const
        {
            return m_metadata[m_indexes[id]].validity_id;
        }

        TObjectType* data()
        {
            return m_data.data();
        }

        std::vector<TObjectType>& getData()
        {
            return m_data;
        }

        const std::vector<TObjectType>& getData() const
        {
            return m_data;
        }

        [[nodiscard]]
        ID getNextID() const
        {
            // This means that we have available slots
            if (m_metadata.size() > m_data.size()) {
                return m_metadata[m_data.size()].rid;
            }
            return m_data.size();;
        }

    private:
        struct Metadata
        {
            ID rid         = 0;
            ID validity_id = 0;
        };

        std::vector<TObjectType> m_data;
        std::vector<Metadata>    m_metadata;
        std::vector<ID>          m_indexes;

        uint64_t operation_count = 0;
    };
}
