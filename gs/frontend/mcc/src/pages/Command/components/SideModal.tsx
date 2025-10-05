import React from 'react'
import SelectCommand from '@/pages/Command/features/components/SelectCommand'
import SendCommand from '@/pages/Command/features/components/SendCommand'


function SideModal() {
  return (
    <div className="p-4 space-y-6 bg-card w-96 border rounded-md">
      <SelectCommand />
      <SendCommand />
    </div>
  )
}

export default SideModal
