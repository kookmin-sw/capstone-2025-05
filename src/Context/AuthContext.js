// AuthContext.js
import { createContext, useContext, useState, useEffect } from 'react';

const AuthContext = createContext();

export const AuthProvider = ({ children }) => {
  const [uid, setUid] = useState(null);

  useEffect(() => {
    const storedUid = sessionStorage.getItem('uid');
    if (storedUid) setUid(storedUid);
  }, []);

  const login = (newUid) => {
    sessionStorage.setItem('uid', newUid);
    setUid(newUid);
  };

  const logout = () => {
    sessionStorage.removeItem('uid');
    setUid(null);
  };

  return (
    <AuthContext.Provider value={{ uid, login, logout }}>
      {children}
    </AuthContext.Provider>
  );
};

export const useAuth = () => useContext(AuthContext);
