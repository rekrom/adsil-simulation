#pragma once

#include <string>
#include <fstream>
#include <memory>
#include <mutex>

namespace utils
{
    /**
     * @brief Exports simulation data to CSV files for post-processing.
     *
     * Thread-safe singleton-style exporter that writes detected ADSIL points
     * to a CSV file with columns: frame, timestamp, transmitter, x, y, z
     */
    class DataExporter
    {
    public:
        static DataExporter &getInstance();

        // Disable copy/move
        DataExporter(const DataExporter &) = delete;
        DataExporter &operator=(const DataExporter &) = delete;
        DataExporter(DataExporter &&) = delete;
        DataExporter &operator=(DataExporter &&) = delete;

        /**
         * @brief Initialize the exporter with output directory path.
         * @param outputDir Directory where CSV files will be written.
         * @return true if initialization succeeded, false otherwise.
         */
        bool init(const std::string &outputDir);

        /**
         * @brief Start a new export session (creates new file with timestamp).
         * @return true if file was created successfully.
         */
        bool startSession();

        /**
         * @brief Set the current frame context for subsequent point exports.
         * @param frameIndex Current frame number.
         * @param timestamp Current simulation timestamp.
         */
        void setFrameContext(int frameIndex, double timestamp);

        /**
         * @brief Export a detected point for the current frame.
         * @param transmitterName Name of the transmitter that detected the point.
         * @param x X coordinate.
         * @param y Y coordinate.
         * @param z Z coordinate.
         */
        void exportPoint(const std::string &transmitterName, float x, float y, float z);

        /**
         * @brief Finalize and close the export file.
         */
        void endSession();

        /**
         * @brief Check if exporter is currently active.
         */
        bool isActive() const { return isActive_; }

        /**
         * @brief Get the path to the current export file.
         */
        std::string getCurrentFilePath() const { return currentFilePath_; }

    private:
        DataExporter() = default;
        ~DataExporter();

        std::string generateFileName() const;

        std::ofstream file_;
        std::string outputDir_;
        std::string currentFilePath_;
        bool isActive_ = false;
        bool isInitialized_ = false;

        // Current frame context
        int currentFrameIndex_ = 0;
        double currentTimestamp_ = 0.0;

        mutable std::mutex mutex_;
    };

} // namespace utils
