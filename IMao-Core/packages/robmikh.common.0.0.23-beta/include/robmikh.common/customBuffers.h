#pragma once
#include <robuffer.h>
#include <winrt/Windows.Storage.Streams.h>

namespace robmikh::common::uwp
{
    struct ComArrayBuffer : winrt::implements<ComArrayBuffer, winrt::Windows::Storage::Streams::IBuffer, ::Windows::Storage::Streams::IBufferByteAccess>
    {
        winrt::com_array<uint8_t> m_buffer;

        ComArrayBuffer(winrt::com_array<uint8_t>&& buffer) :
            m_buffer(std::move(buffer))
        {
        }

        uint32_t Capacity() const
        {
            return m_buffer.size();
        }

        uint32_t Length() const
        {
            return m_buffer.size();
        }

        void Length(uint32_t)
        {
            throw winrt::hresult_access_denied();
        }

        HRESULT __stdcall Buffer(uint8_t** value) final
        {
            *value = m_buffer.data();
            return S_OK;
        }
    };
}