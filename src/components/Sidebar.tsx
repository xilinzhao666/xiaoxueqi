import React from 'react'
import { NavLink } from 'react-router-dom'
import { 
  LayoutDashboard, 
  UserCheck, 
  Users, 
  Calendar, 
  FileText, 
  Pill, 
  Building2 
} from 'lucide-react'

const navigation = [
  { name: '仪表板', href: '/dashboard', icon: LayoutDashboard },
  { name: '医生管理', href: '/doctors', icon: UserCheck },
  { name: '患者管理', href: '/patients', icon: Users },
  { name: '预约管理', href: '/appointments', icon: Calendar },
  { name: '病例管理', href: '/cases', icon: FileText },
  { name: '处方管理', href: '/prescriptions', icon: Pill },
  { name: '住院管理', href: '/hospitalization', icon: Building2 },
]

const Sidebar: React.FC = () => {
  return (
    <aside className="w-64 bg-white border-r border-gray-200 min-h-screen">
      <nav className="p-6 space-y-2">
        {navigation.map((item) => (
          <NavLink
            key={item.name}
            to={item.href}
            className={({ isActive }) =>
              `flex items-center space-x-3 px-4 py-3 rounded-lg text-sm font-medium transition-colors ${
                isActive
                  ? 'bg-primary-50 text-primary-700 border border-primary-200'
                  : 'text-gray-700 hover:bg-gray-50 hover:text-gray-900'
              }`
            }
          >
            <item.icon className="w-5 h-5" />
            <span>{item.name}</span>
          </NavLink>
        ))}
      </nav>
    </aside>
  )
}

export default Sidebar