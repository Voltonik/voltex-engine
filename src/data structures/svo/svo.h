#include <vk_types.h>

struct VoxelData {
    glm::vec3 color;
};

struct Node {
    bool IsLeaf;
    Node* children[8];
    VoxelData data;

    Node() : IsLeaf(false) {
        for (int i = 0; i < 8; i++) {
            children[i] = nullptr;
        }
    }
};

class SparseVoxelOctree {
private:
    Node* m_Root;
    int m_Size, m_MaxDepth, m_VoxelCount;
    std::vector<uint8_t> colors;

    void Insert(Node** node, glm::vec3 point, glm::vec3 color, glm::ivec3 parentCenter, int depth);
    uint32_t CreateDescriptor(Node* node, int& index, int pIndex);
    void CreateBuffer(Node* node, int& index);

public:
    std::vector<uint32_t> m_Buffer, m_Far;

    SparseVoxelOctree(int size, int maxDepth);

    void Insert(glm::vec3 point, glm::vec3 color);
    void CreateBuffer();

    uint32_t GetBufferSize() const { return m_Buffer.size() * sizeof(uint32_t); }
    uint32_t GetFarBufferSize() const { return m_Far.size() * sizeof(uint32_t); }
};