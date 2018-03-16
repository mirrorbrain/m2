#include "api.hpp"

#include <string>
#include <fstream>

#include "json.hpp"
#include "aes.hpp"

API::API(const char *fileUsers, const char *fileKeys) : _connectedUser(NULL)
{
        myerror_t cr = NO_ERROR;
        std::filebuf fb;
        std::istream is(&fb);
        _isInit = false;
        /** <ol> */
        /** <li> Check that the user file exists */
        if ( !fb.open(fileUsers,std::ios::in) )
        {
                std::cerr << "[error] File " << fileUsers << " cannot be opened !" << std::endl;
        }
        else
        {
                fb.close();
                /** <li> Check that the key file exists */
                if ( !fb.open(fileKeys,std::ios::in) )
                {
                        std::cerr << "[error] File " << fileKeys << " cannot be opened !" << std::endl;
                }
                else
                {
                        fb.close();
                        /** <li> If yes copy filenames to API#fileUsers and API#fileKeys */
                        this->_fileUsers = fileUsers;
                        this->_fileKeys = fileKeys;
                        /** <li> Call API#loadAPI() to load data from the file */
                        cr = loadAPI();
                        /** <li> Test return code and set API::isInit in case of success */
                        if ( cr == NO_ERROR )
                        {
                                this->_isInit = true;
                        }
                }
        }
        /** </ol> */
}

API::~API()
{
        /** <ol> */
        /** <li> Do nothing if API is not initialized </li> */
        if ( _isInit )
        {
                /** <li> Dump API to the file API#filename </li> */
                saveAPI();
        }
        /** <li> Free all classes using reset() function. </li> */
        reset();
        /** </ol> */
}

void API::reset()
{
        if ( NULL != _connectedUser )
                disconnect();

        for ( auto it = _users.begin(); it != _users.end(); it++ )
                delete *it;
        _users.clear();

        for ( auto it = _keys.begin(); it != _keys.end(); it++ )
                delete *it;
        _keys.clear();

        User *admin = new User("admin","s3cr3t_p@ss",1,USER_ADMIN);
        myerror_t cr = addUser(admin);
        if ( NO_ERROR != cr )
        {
                std::cerr << "[error] Cannot create admin account." << std::endl;
        }

}

myerror_t API::newUser(const std::string &login, const std::string &password, uint32_t *id)
{
        myerror_t cr = NO_ERROR;
        User *user = NULL;

        if ( NULL != id )
                *id = 0;
        /* Test somebody is connected */
        if ( NULL == _connectedUser )
        {
                std::cerr << "[error] Admin should be connected to create a user." << std::endl;
                cr = ERROR_SEQ;
        }

        /* Test for admin rights */
        if ( NO_ERROR == cr )
        {
                if ( USER_ADMIN != _connectedUser->getRole() )
                {
                        std::cerr << "[error] Cannot create user without admin rights." << std::endl;
                        cr = ERROR_SEC;
                }
        }

        /* If ok create user */
        if ( NO_ERROR == cr )
        {
                uint32_t new_id = _users.back()->getId() + 1;
                user = new User(login.c_str(),password.c_str(),new_id,USER_BASIC);
                cr = addUser(user);
        }
        if ( ERROR_PARAM == cr )
        {
                std::cerr << "[error] Cannot create already existing user." << std::endl;
        }
        if ( NO_ERROR == cr )
        {
                std::cout << "[info] User " << login << " created." << std::endl;
                if ( NULL != id )
                        *id = user->getId();
        }
        return cr;
}

myerror_t API::deleteUser(uint32_t id)
{
        myerror_t cr = NO_ERROR;

        /* Test somebody is connected */
        if ( NULL == _connectedUser )
        {
                std::cerr << "[error] Admin should be connected to delete a user." << std::endl;
                cr = ERROR_SEQ;
        }

        /* Test for admin rights */
        if ( NO_ERROR == cr )
        {
                if ( USER_ADMIN != _connectedUser->getRole() )
                {
                        std::cerr << "[error] Cannot create user without admin rights." << std::endl;
                        cr = ERROR_SEC;
                }
        }

        /* If ok delete user */
        if ( NO_ERROR == cr )
        {
                auto it = _users.begin();
                for (; it != _users.end(); it++ )
                {
                        if ( (*it)->getId() == id ) // found
                                break;
                }
                if ( it == _users.end() )
                {
                        cr = ERROR_PARAM;
                }
                else
                {
                        std::cerr << "[info] User " << (*it)->getLogin() << " deleted." << std::endl;
                        _users.erase(it);
                        /* Erase corresponding keys */
                        for ( auto it = _keys.begin(); it != _keys.end(); )
                        {
                                if ( (*it)->_owner == id )
                                {
                                        printf("[info] Erasing key %d\n",(*it)->_id);
                                        _keys.erase(it);
                                }
                                else it++;
                        }
                }
        }

        return cr;
}

myerror_t API::newKey(key_type_t type, key_use_t use, date_t validity, size_t length, uint32_t *id)
{
        myerror_t cr = NO_ERROR;

        if ( NULL != id )
                *id = 0;
        /** <li> Only symmetric keys are implemented at the moment </li> */
        if ( KEY_SYM != type )
        {
                cr = ERROR_PARAM;
                std::cerr << "[error] Only symmetric keys can be generated in this version of the API." << std::endl;
        }
        else
        {
                /** <li> Check key size </li> */
                if ( (0 == length) || ( MAX_KEY_LEN < length) )
                        cr = ERROR_PARAM;
                /** <li> Check use </li> */
                else if ( (KEY_ENCR != use) &&
                          (KEY_SIGN != use) &&
                          (KEY_INTE != use) &&
                          (KEY_WRAP != use) )
                {
                        cr = ERROR_PARAM;
                }
        }
        if ( NO_ERROR == cr )
        {
                Key *newkey = new Key();
                /** <li> Setting attributes </li> */
                newkey->_type = type;
                newkey->_use = use;
                newkey->_validity = validity;
                newkey->_length = length;
                newkey->_associated = 0; // symmetric keys have no associated key
                newkey->_owner = _connectedUser->getId();
                newkey->_id = _keys.back()->_id + 1;
                /** <li> Generating the value </li> */
                newkey->_value.clear();
                srand(time(NULL)+newkey->_id);
                for ( unsigned i = 0; i < length; i++ )
                {
                        uint8_t byte = rand() & 256; // get a value between 0 and 255
                        newkey->_value.push_back(byte);
                }
                addKey(newkey);
                if ( NULL != id )
                        *id = newkey->_id;
                std::cout << "[info] Key " << newkey->_id << " has been created." << std::endl;
        }

        return cr;
}

myerror_t API::disconnect()
{
        myerror_t cr = NO_ERROR;
        if ( NULL == _connectedUser )
        {
                std::cerr << "[error] No user to disconnect." << std::endl;
                cr = ERROR_SEQ;
        }
        else
        {
                std::cout << "[info] User " << _connectedUser->getLogin() << " is now disconnected." << std::endl;
                _connectedUser = NULL;
        }

        return cr;
}

myerror_t API::connectUser(const std::string &login, const std::string &password)
{
        myerror_t cr = NO_ERROR;
        unsigned i;

        /* Test if API is initialized */
        if ( false == _isInit )
        {
                cr = ERROR_SEQ;
        }
        /* Test that no users are currently connected */
        if ( NULL != _connectedUser )
        {
                std::cerr << "[error] There already is a connected user." << std::endl;
                cr = ERROR_SEQ;
        }

        /* Find user in the base */
        if ( NO_ERROR == cr )
        {
                for ( i = 0; i < _users.size(); i++ )
                {
                        if ( _users[i]->hasSameLogin(login) )
                        {
                                break;
                        }
                }
                if ( _users.size() == i )
                {
                        std::cerr << "[error] User " << login << " not found." << std::endl;
                        cr = ERROR_PARAM;
                }
        }

        /* Check password */
        if ( NO_ERROR == cr )
        {
                if ( true == _users[i]->isCorrectPassword(password) )
                {
                        /* Connect it if ok */
                        _connectedUser = _users[i];
                        std::cout << "[info] User " << login << " is now connected." << std::endl;
                }
                else
                {
                        std::cerr << "[error] Bad password." << std::endl;
                }
        }

        return cr;
}

myerror_t API::wrap(uint32_t idWrapKey, uint32_t idExportedKey, uint8_t *output, size_t *length)
{
        /** <ol> */
        myerror_t ret = NO_ERROR;
        Key *wrap = NULL, *exported = NULL;
        uint8_t wrap_key[MAX_KEY_LEN], exported_key[MAX_KEY_LEN];
        uint8_t bloc[16];
        size_t allocatedLength = MAX_KEY_LEN;

        memset(wrap_key,0,allocatedLength);
        memset(exported_key,0,allocatedLength);

        /** <li> Get the keys </li> */
        if ( (findKey(idWrapKey,&wrap) != NO_ERROR) ||
             (findKey(idExportedKey,&exported) != NO_ERROR) )
        {
                ret = ERROR_PARAM;
        }
        /** <li> Test that the connected user owns the keys </li> */
        if ( ret == NO_ERROR )
        {
                if ( (wrap->getOwner() != _connectedUser->getId()) ||
                     (exported->getOwner() != _connectedUser->getId()) )
                {
                        ret = ERROR_SEC;
                }
        }
        /** <li> Test wrap key use and output size </li> */
        if ( ret == NO_ERROR )
        {
                if ( wrap->getUse() != KEY_WRAP )
                        ret = ERROR_PARAM;
                if ( *length < exported->_length )
                        ret = ERROR_PARAM;
        }
        /** <li> Wrap key </li> */
        if ( ret = NO_ERROR )
        {
                decryptKey(wrap,wrap_key,&allocatedLength);
                decryptKey(exported,exported_key,&allocatedLength);

                size_t index = 0;
                do
                {
                        AES::encrypt(wrap_key,exported_key+index,bloc);
                        for ( unsigned i = 0; i < 16; i++ )
                        {
                                if ( index >= exported->_length )
                                        break;
                                output[index] = bloc[i];
                                index++;
                        }
                }
                while(index < exported->_length);

                *length = exported->_length;
        }

        /** </ol> */
        return ret;
}

myerror_t API::unwrap(uint32_t idWrapKey, uint8_t *importedKey, size_t length, key_type_t type, key_use_t use, date_t validity, uint32_t *id)
{
        /** <ol> */
        myerror_t ret = NO_ERROR;
        Key *wrap = NULL;
        uint8_t wrap_key[MAX_KEY_LEN], imported_key[MAX_KEY_LEN];
        uint8_t bloc[16];
        size_t allocatedLength = MAX_KEY_LEN;

        memset(wrap_key,0,allocatedLength);
        memset(imported_key,0,allocatedLength);

        /** <li> Get the key </li> */
        if ( NO_ERROR != findKey(idWrapKey,&wrap) )
        {
                ret = ERROR_PARAM;
        }
        /** <li> Test that the connected user owns the key </li> */
        if ( NO_ERROR == ret )
        {
                if ( wrap->getOwner() != _connectedUser->getId() )
                {
                        ret = ERROR_SEC;
                }
        }
        /** <li> Test wrap key use and import size </li> */
        if ( NO_ERROR == ret )
        {
                if ( KEY_WRAP != wrap->getUse() )
                        ret = ERROR_PARAM;
                if ( length > MAX_KEY_LEN )
                        ret = ERROR_PARAM;
        }
        /** <li> Unwrap key and protect it with local key </li> */
        if ( NO_ERROR == ret )
        {
                decryptKey(wrap,wrap_key,&allocatedLength);

                size_t index = 0;
                do
                {
                        AES::decrypt(wrap_key,importedKey+index,bloc);
                        for ( unsigned i = 0; i < 16; i++ )
                        {
                                if ( index >= length )
                                        break;
                                imported_key[index] = bloc[i];
                                index++;
                        }
                }
                while(index < length);

        }
        /** <li> Add key to API </li> */
        Key *newkey = new Key();
        /** <li> Setting attributes </li> */
        newkey->_type = type;
        newkey->_use = use;
        newkey->_validity = validity;
        newkey->_length = length;
        newkey->_associated = 0; // symmetric keys have no associated key
        newkey->_owner = _connectedUser->getId();
        newkey->_id = _keys.back()->_id + 1;
        newkey->_value.clear();
        protectData(imported_key,length);
        for ( unsigned i = 0; i < length; i++ )
        {
                newkey->_value.push_back(imported_key[i]);
        }
        addKey(newkey);
        if ( NULL != id )
                *id = newkey->_id;
        std::cout << "[info] Key " << newkey->_id << " has been imported." << std::endl;

        /** </ol> */
        return ret;
}

myerror_t API::encrypt(uint32_t idKey, uint8_t *buffer, size_t length)
{
        /** <ol> */
        myerror_t ret = NO_ERROR;
        Key *encryption = NULL;
        uint8_t encryption_key[MAX_KEY_LEN];
        uint8_t bloc[16];
        size_t allocatedLength = MAX_KEY_LEN;

        memset(encryption_key,0,allocatedLength);

        /** <li> Get the key </li> */
        if ( NO_ERROR != findKey(idKey,&encryption) )
        {
                ret = ERROR_PARAM;
        }
        /** <li> Test that the connected user owns the key </li> */
        if ( NO_ERROR == ret )
        {
                if ( encryption->getOwner() != _connectedUser->getId() )
                {
                        ret = ERROR_SEC;
                }
        }
        /** <li> Test key use and type</li> */
        if ( NO_ERROR == ret )
        {
                if ( (KEY_ENCR != encryption->getUse()) ||
                     (KEY_SYM != encryption->getType()) )
                        ret = ERROR_PARAM;
        }
        /** <li> Encrypt </li> */
        if ( NO_ERROR == ret )
        {
                decryptKey(encryption,encryption_key,&allocatedLength);

                size_t index = 0;
                do
                {
                        AES::encrypt(encryption_key,buffer+index,bloc);
                        for ( unsigned i = 0; i < 16; i++ )
                        {
                                if ( index >= length )
                                        break;
                                buffer[index] = bloc[i];
                                index++;
                        }
                }
                while(index < length);
        }

        /** </ol> */
        return ret;
}

myerror_t API::decrypt(uint32_t idKey, uint8_t *buffer, size_t length)
{
        /** <ol> */
        myerror_t ret = NO_ERROR;
        Key *decryption = NULL;
        uint8_t decryption_key[MAX_KEY_LEN];
        uint8_t bloc[16];
        size_t allocatedLength = MAX_KEY_LEN;

        memset(decryption_key,0,allocatedLength);

        /** <li> Get the key </li> */
        if ( NO_ERROR != findKey(idKey,&decryption) )
        {
                ret = ERROR_PARAM;
        }
        /** <li> Test that the connected user owns the key </li> */
        if ( NO_ERROR == ret )
        {
                if ( decryption->getOwner() != _connectedUser->getId() )
                {
                        ret = ERROR_SEC;
                }
        }
        /** <li> Test key use and type</li> */
        if ( NO_ERROR == ret )
        {
                if ( (KEY_ENCR != decryption->getUse()) ||
                     (KEY_SYM != decryption->getType()) )
                        ret = ERROR_PARAM;
        }
        /** <li> Encrypt </li> */
        if ( NO_ERROR == ret )
        {
                decryptKey(decryption,decryption_key,&allocatedLength);

                size_t index = 0;
                do
                {
                        AES::decrypt(decryption_key,buffer+index,bloc);
                        for ( unsigned i = 0; i < 16; i++ )
                        {
                                if ( index >= length )
                                        break;
                                buffer[index] = bloc[i];
                                index++;
                        }
                }
                while(index < length);
        }

        /** </ol> */
        return ret;
}

void API::initFile(const char *fileUsers, const char *fileKeys, unsigned instanceId)
{
        std::filebuf fb;
        std::ostream os(&fb);

        if ( !fb.open (fileUsers,std::ios::out) )
        {
                std::cerr << "[error] File " << fileUsers << " cannot be opened !" << std::endl;
        }
        else
        {
                os << "{" << std::endl;
                os << "\t\"instance\": " << instanceId << "," << std::endl;
                os << "\t\"users\": [" << std::endl;

                /* adding one administrator */

                /*
                   os << "\t\t{ \"id\": 1 , \"login\": \"admin\" , \"password\": \"+uEKI58lNaEiAfmVVuB8Y2GuUIZjHiMKAUlmIGN0t7I=\" , \"privilege\": 1 }" << std::endl;
                   os << "\t]," << std::endl;
                   os << "\"tag\": \"NWFKoK+Qc3PMpMgHydUuls9RfozXn9IRP6wqnxiynNY=\"" << std::endl;
                   /*/
                os << "\t\t{ \"id\": 1 , \"login\": \"admin\" , \"password\": \"+uEKI58lNaEiAfmVVuB8Y2GuUIZjHiMKAUlmIGN0t7I=\" , \"privilege\": 1 }," << std::endl;
                os << "\t\t{ \"id\": 2 , \"login\": \"benoit\" , \"password\": \"9y6911j6NJHeB5LnjBKqzx3SvAmkiQ6a+yzelxSRZ18=\" , \"privilege\": 0 }" << std::endl;
                os << "\t]," << std::endl;
                os << "\"tag\": \"lL7h/4xRHzWq3vYnG2F1Lq41otRjNUmXumGY1aGwZcg=\"" << std::endl;
                //*/

                os << "}" << std::endl;

                fb.close();
        }

        if ( !fb.open (fileKeys,std::ios::out) )
        {
                std::cerr << "[error] File " << fileKeys << " cannot be opened !" << std::endl;
        }
        else
        {
                os << "{" << std::endl;
                os << "\t\"instance\": " << instanceId << "," << std::endl;
                os << "\t\"keys\": [" << std::endl;
                os << "\t\t{ \"id\": 1 , \"owner\": 2 , \"type\": 1 , \"use\": 4 , \"validity\": 2020 , \"length\": 16 , \"value\": \"77ZinlHFcCkBn77fYOYG0g==\" }," << std::endl;
                os << "\t\t{ \"id\": 2 , \"owner\": 2 , \"type\": 1 , \"use\": 1 , \"validity\": 2020 , \"length\": 16 , \"value\": \"acTg2Gp7BDDYzbeAcLTFWg==\" }" << std::endl;
                os << "\t]," << std::endl;
                os << "\"tag\": \"25BiMogj15hjGpOqflVpb+8vV/6ZWkPprOgQYATsnmk=\"" << std::endl;
                os << "}" << std::endl;

                fb.close();
        }
}
