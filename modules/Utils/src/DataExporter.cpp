#include "utils/DataExporter.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <filesystem>

namespace utils
{
    DataExporter &DataExporter::getInstance()
    {
        static DataExporter instance;
        return instance;
    }

    DataExporter::~DataExporter()
    {
        endSession();
    }

    bool DataExporter::init(const std::string &outputDir)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        outputDir_ = outputDir;

        // Ensure output directory exists
        try
        {
            if (!std::filesystem::exists(outputDir_))
            {
                std::filesystem::create_directories(outputDir_);
            }
            isInitialized_ = true;
            return true;
        }
        catch (const std::filesystem::filesystem_error &e)
        {
            std::cerr << "[DataExporter] Failed to create output directory: " << e.what() << std::endl;
            return false;
        }
    }

    std::string DataExporter::generateFileName() const
    {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time);

        std::ostringstream oss;
        oss << "detected_points_"
            << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S")
            << ".csv";

        return oss.str();
    }

    bool DataExporter::startSession()
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!isInitialized_)
        {
            std::cerr << "[DataExporter] Not initialized. Call init() first." << std::endl;
            return false;
        }

        if (isActive_)
        {
            // Close existing session
            file_.close();
        }

        currentFilePath_ = outputDir_ + "/" + generateFileName();

        file_.open(currentFilePath_, std::ios::out | std::ios::trunc);
        if (!file_.is_open())
        {
            std::cerr << "[DataExporter] Failed to open file: " << currentFilePath_ << std::endl;
            return false;
        }

        // Write CSV header
        file_ << "frame,timestamp,transmitter,x,y,z\n";
        file_.flush();

        isActive_ = true;
        currentFrameIndex_ = 0;
        currentTimestamp_ = 0.0;

        std::cout << "[DataExporter] Started export session: " << currentFilePath_ << std::endl;
        return true;
    }

    void DataExporter::setFrameContext(int frameIndex, double timestamp)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        currentFrameIndex_ = frameIndex;
        currentTimestamp_ = timestamp;
    }

    void DataExporter::exportPoint(const std::string &transmitterName, float x, float y, float z)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!isActive_ || !file_.is_open())
        {
            return;
        }

        // Write CSV row: frame,timestamp,transmitter,x,y,z
        file_ << currentFrameIndex_ << ","
              << std::fixed << std::setprecision(6) << currentTimestamp_ << ","
              << transmitterName << ","
              << std::fixed << std::setprecision(6) << x << ","
              << std::fixed << std::setprecision(6) << y << ","
              << std::fixed << std::setprecision(6) << z << "\n";

        // Flush periodically for reliability (every point for now, could optimize)
        file_.flush();
    }

    void DataExporter::endSession()
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (isActive_ && file_.is_open())
        {
            file_.close();
            std::cout << "[DataExporter] Export session ended: " << currentFilePath_ << std::endl;
        }

        isActive_ = false;
    }

} // namespace utils
