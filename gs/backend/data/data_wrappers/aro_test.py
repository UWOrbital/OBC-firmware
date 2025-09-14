from aro_wrapper.aro_user_data_wrapper import add_user
from aro_wrapper.aro_user_login_wrapper import add_login, get_all_logins

if __name__ == "__main__":
    # basic test to create two new users and their login credentials
    new_user = add_user(
        call_sign="KEVWAN", email="kevian@gmail.com", f_name="kevin", l_name="wan", phone_number="8888888888"
    )
    second_user = add_user(
        call_sign="BEVWAN", email="bevian@gmail.com", f_name="bevin", l_name="ban", phone_number="9999999999"
    )

    login = add_login(new_user.email, "lahahhaha", "kevalgo", new_user.id, "abbababababa")
    add_login(second_user.email, "klajal", "kevalgoadvanced", second_user.id, "abbababababa")

    print(login)
    print(get_all_logins())
