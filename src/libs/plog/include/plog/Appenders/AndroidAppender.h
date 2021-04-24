#pragma once
#include <plog/Appenders/IAppender.h>
#include <android/log.h>

namespace plog
{
    template<class Formatter>
    class AndroidAppender : public IAppender
    {
    public:
        AndroidAppender(const char* tag) : m_tag(tag)
        {
        }

        virtual void write(const Record& record)
        {
            std::string str = Formatter::format(record);

            __android_PLOG_print(toPriority(record.getSeverity()), m_tag, "%s", str.c_str());
        }

    private:
        static android_LogPriority toPriority(Severity severity)
        {
            switch (severity)
            {
            case fatal:
                return ANDROID_PLOG_FATAL;
            case error:
                return ANDROID_PLOG_ERROR;
            case warning:
                return ANDROID_PLOG_WARN;
            case info:
                return ANDROID_PLOG_INFO;
            case debug:
                return ANDROID_PLOG_DEBUG;
            case verbose:
                return ANDROID_PLOG_VERBOSE;
            default:
                return ANDROID_PLOG_UNKNOWN;
            }
        }

    private:
        const char* const m_tag;
    };
}
