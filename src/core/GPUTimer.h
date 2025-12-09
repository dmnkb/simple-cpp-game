#pragma once

#include <glad/glad.h>
#include <map>
#include <string>
#include <vector>

#include "renderer/GLDebug.h"

namespace Engine
{

/**
 * GPU Timer using OpenGL timer queries.
 * Uses double-buffering to avoid GPU stalls when reading query results.
 * Results are retrieved from the previous frame (industry standard practice).
 */
class GPUTimer
{
  public:
    GPUTimer() = default;
    ~GPUTimer()
    {
        cleanup();
    }

    // Non-copyable
    GPUTimer(const GPUTimer&) = delete;
    GPUTimer& operator=(const GPUTimer&) = delete;

    /**
     * Begin timing a GPU region. Must be paired with end().
     */
    void begin(const std::string& regionName)
    {
        ensureQuery(regionName);
        auto& query = m_queries[regionName];
        GLCall(glBeginQuery(GL_TIME_ELAPSED, query.ids[m_writeBuffer]));
    }

    /**
     * End timing a GPU region.
     */
    void end(const std::string& regionName)
    {
        GLCall(glEndQuery(GL_TIME_ELAPSED));
    }

    /**
     * Swap read/write buffers and retrieve results from the read buffer.
     * Call this at the end of each frame.
     */
    void swapBuffers()
    {
        // Retrieve results from read buffer (previous frame)
        m_results.clear();
        for (const auto& name : m_regionOrder)
        {
            auto it = m_queries.find(name);
            if (it == m_queries.end())
                continue;

            auto& query = it->second;
            GLuint64 elapsed = 0;

            // Check if result is available (non-blocking)
            GLint available = 0;
            GLCall(glGetQueryObjectiv(query.ids[m_readBuffer], GL_QUERY_RESULT_AVAILABLE, &available));
            if (available)
            {
                GLCall(glGetQueryObjectui64v(query.ids[m_readBuffer], GL_QUERY_RESULT, &elapsed));
            }

            // Convert nanoseconds to milliseconds
            double ms = static_cast<double>(elapsed) / 1000000.0;
            m_results.emplace_back(name, ms);
        }

        // Swap buffers
        std::swap(m_readBuffer, m_writeBuffer);
    }

    /**
     * Get GPU timing results from the previous frame.
     * Returns vector of (region_name, time_in_ms) pairs.
     */
    const std::vector<std::pair<std::string, double>>& getResults() const
    {
        return m_results;
    }

  private:
    struct QueryPair
    {
        GLuint ids[2] = {0, 0}; // Double-buffered query objects
    };

    void ensureQuery(const std::string& name)
    {
        if (m_queries.find(name) != m_queries.end())
            return;

        QueryPair query;
        GLCall(glGenQueries(2, query.ids));
        m_queries[name] = query;
        m_regionOrder.push_back(name);
    }

    void cleanup()
    {
        for (auto& [name, query] : m_queries)
        {
            GLCall(glDeleteQueries(2, query.ids));
        }
        m_queries.clear();
        m_regionOrder.clear();
    }

  private:
    std::map<std::string, QueryPair> m_queries;
    std::vector<std::string> m_regionOrder; // Maintains insertion order
    std::vector<std::pair<std::string, double>> m_results;

    int m_readBuffer = 0;
    int m_writeBuffer = 1;
};

} // namespace Engine
