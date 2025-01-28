#include "svo.h"

static int childCount = 0;
static const int PointerOffsetFarMax = std::exp2(15);

SparseVoxelOctree::SparseVoxelOctree(int size, int maxDepth) {
    m_Size = size;
    m_MaxDepth = maxDepth;
    m_Root = nullptr;
}

void SparseVoxelOctree::Insert(glm::vec3 point, glm::vec3 color) {
    point += glm::vec3(m_Size) / glm::vec3(2);
    Insert(&m_Root, point, color, glm::vec3(0), 0);
}

void SparseVoxelOctree::Insert(Node** node, glm::vec3 point, glm::vec3 color, glm::ivec3 parentCenter, int depth) {
    if (*node == nullptr) {
        *node = new Node;
    }

    if (depth == m_MaxDepth) {
        (*node)->IsLeaf = true;
        (*node)->data.color = color;
        return;
    }

    (*node)->data.color = color;

    float size = m_Size / std::exp2(depth);
    glm::ivec3 childPos = glm::vec3(
        (int)std::round((point.x - ((float)parentCenter.x * size)) / size),
        (int)std::round((point.y - ((float)parentCenter.y * size)) / size),
        (int)std::round((point.z - ((float)parentCenter.z * size)) / size)
    );

    int childIndex = (childPos.x << 0) | (childPos.y << 1) | (childPos.z << 2);

    parentCenter = glm::ivec3(
        (parentCenter.x << 1) | childPos.x,
        (parentCenter.y << 1) | childPos.y,
        (parentCenter.z << 1) | childPos.z
    );

    Insert(&(*node)->children[childIndex], point, color, parentCenter, ++depth);
}

void SparseVoxelOctree::CreateBuffer() {
    int i = 0;
    CreateBuffer(m_Root, i);
}

void SparseVoxelOctree::CreateBuffer(Node* node, int& index) {
    if (node == m_Root)
        m_Buffer.push_back(CreateDescriptor(node, ++index, 0));

    int pIndex = index - childCount;
    m_Buffer.resize(index);
    for (int i = 0; i < 8; i++) {
        if (Node* child = node->children[i]) {
            if (!child->IsLeaf) {
                m_Buffer.at(pIndex) = CreateDescriptor(child, index, pIndex);
                pIndex++;
                CreateBuffer(child, index);
            }
        }
    }
}

uint32_t SparseVoxelOctree::CreateDescriptor(Node* node, int& index, int pIndex) {
    uint32_t childDesc = 0;
    int ValidChildCount = 0;
    for (int i = 0; i < 8; i++) {
        if (Node* child = node->children[i]) {
            childDesc |= 1 << i;
            if (child->IsLeaf) {
                childDesc |= 1 << (i + 8);
                m_VoxelCount++;
            }
            else if (ValidChildCount == 0) {
                int indexOffset = index - pIndex;
                if (indexOffset >= PointerOffsetFarMax) {
                    childDesc |= 1 << 16;
                    m_Far.push_back(indexOffset);
                    childDesc |= m_Far.size() - 1 << 17;
                }
                else
                    childDesc |= indexOffset << 17;
            }
            ValidChildCount++;
        }
    }

    childCount = ValidChildCount;
    index += ValidChildCount;
    return childDesc;
}